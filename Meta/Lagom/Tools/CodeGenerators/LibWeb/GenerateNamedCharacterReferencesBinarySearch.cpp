/*
 * Copyright (c) 2024, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "GeneratorUtil.h"
#include <AK/Array.h>
#include <AK/CharacterTypes.h>
#include <AK/FixedArray.h>
#include <AK/SourceGenerator.h>
#include <AK/StringBuilder.h>
#include <LibCore/ArgsParser.h>
#include <LibMain/Main.h>

ErrorOr<void> generate_header_file(Core::File& file);
ErrorOr<void> generate_implementation_file(JsonObject& named_character_reference_data, Core::File& file);

ErrorOr<int> serenity_main(Main::Arguments arguments)
{
    StringView generated_header_path;
    StringView generated_implementation_path;
    StringView json_path;

    Core::ArgsParser args_parser;
    args_parser.add_option(generated_header_path, "Path to the Entities header file to generate", "generated-header-path", 'h', "generated-header-path");
    args_parser.add_option(generated_implementation_path, "Path to the Entities implementation file to generate", "generated-implementation-path", 'c', "generated-implementation-path");
    args_parser.add_option(json_path, "Path to the JSON file to read from", "json-path", 'j', "json-path");
    args_parser.parse(arguments);

    auto json = TRY(read_entire_file_as_json(json_path));
    VERIFY(json.is_object());
    auto named_character_reference_data = json.as_object();

    auto generated_header_file = TRY(Core::File::open(generated_header_path, Core::File::OpenMode::Write));
    auto generated_implementation_file = TRY(Core::File::open(generated_implementation_path, Core::File::OpenMode::Write));

    TRY(generate_header_file(*generated_header_file));
    TRY(generate_implementation_file(named_character_reference_data, *generated_implementation_file));

    return 0;
}

struct Codepoints {
    u32 first;
    u32 second;
};

ErrorOr<void> generate_header_file(Core::File& file)
{
    StringBuilder builder;
    SourceGenerator generator { builder };
    generator.append(R"~~~(
#pragma once

#include <AK/Optional.h>
#include <AK/Types.h>
#include <LibWeb/HTML/Parser/NamedCharacterReferences.h>

namespace Web::HTML {

struct FirstLayerNode {
    // Really only needs 12 bits.
    u16 number;
    u16 child_index : 10;
    u8 children_len : 5;
};
static_assert(sizeof(FirstLayerNode) == 4);

struct NamedCharacterReferenceNode {
    // The actual alphabet of characters used in the list of named character references only
    // includes 61 unique characters ('1'...'8', ';', 'a'...'z', 'A'...'Z'), but we have
    // bits to spare and encoding this as a `u7` allows us to avoid the need for converting
    // between an `enum(u6)` containing only the alphabet and the actual `u7` character value.
    u8 character : 7;
    bool end_of_word : 1;
    // Nodes are numbered with "an integer which gives the number of words that
    // would be accepted by the automaton starting from that state." This numbering
    // allows calculating "a one-to-one correspondence between the integers 1 to L
    // (L is the number of words accepted by the automaton) and the words themselves."
    //
    // Essentially, this allows us to have a minimal perfect hashing scheme such that
    // it's possible to store & lookup the codepoint transformations of each named character
    // reference using a separate array.
    //
    // Empirically, the largest number in our DAFSA is 168, so all number values fit in a u8.
    u8 number;
    // Index of the first child of this node.
    // There are 3872 nodes in our DAFSA, so all indexes can fit in a u12.
    u16 child_index : 12;
    u8 children_len : 4;
};
static_assert(sizeof(NamedCharacterReferenceNode) == 4);

extern NamedCharacterReferenceNode g_named_character_reference_nodes[];
extern FirstLayerNode g_named_character_reference_first_layer[];

} // namespace Web::HTML

)~~~");

    TRY(file.write_until_depleted(generator.as_string_view().bytes()));
    return {};
}

class Node final : public RefCounted<Node> {
private:
    struct NonnullRefPtrNodeTraits {
        static unsigned hash(NonnullRefPtr<Node> const& node)
        {
            u32 hash = 0;
            for (int i = 0; i < 128; i++) {
                hash ^= ptr_hash(node->m_children[i].ptr());
            }
            hash ^= int_hash(static_cast<u32>(node->m_is_terminal));
            return hash;
        }
        static bool equals(NonnullRefPtr<Node> const& a, NonnullRefPtr<Node> const& b)
        {
            if (a->m_is_terminal != b->m_is_terminal)
                return false;
            for (int i = 0; i < 128; i++) {
                if (a->m_children[i] != b->m_children[i])
                    return false;
            }
            return true;
        }
    };

public:
    static NonnullRefPtr<Node> create()
    {
        return adopt_ref(*new (nothrow) Node());
    }

    using NodeTableType = HashTable<NonnullRefPtr<Node>, NonnullRefPtrNodeTraits, false>;

    void calc_numbers()
    {
        m_number = static_cast<u16>(m_is_terminal);
        for (int i = 0; i < 128; i++) {
            if (m_children[i] == nullptr)
                continue;
            m_children[i]->calc_numbers();
            m_number += m_children[i]->m_number;
        }
    }

    u8 num_direct_children()
    {
        u8 num = 0;
        for (int i = 0; i < 128; i++) {
            if (m_children[i] != nullptr)
                num += 1;
        }
        return num;
    }

    Array<RefPtr<Node>, 128>& children() { return m_children; }

    void set_as_terminal() { m_is_terminal = true; }

    bool is_terminal() const { return m_is_terminal; }

    u16 number() const { return m_number; }

private:
    Node() = default;

    Array<RefPtr<Node>, 128> m_children { 0 };
    bool m_is_terminal { false };
    u16 m_number { 0 };
};

struct UncheckedNode {
    RefPtr<Node> parent;
    char character;
    RefPtr<Node> child;
};

class DafsaBuilder {
    AK_MAKE_NONCOPYABLE(DafsaBuilder);

public:
    using MappingType = HashMap<StringView, String>;

    DafsaBuilder()
        : m_root(Node::create())
    {
    }

    void insert(StringView str)
    {
        // Must be inserted in sorted order
        VERIFY(str > m_previous_word);

        size_t common_prefix_len = 0;
        for (size_t i = 0; i < min(str.length(), m_previous_word.length()); i++) {
            if (str[i] != m_previous_word[i])
                break;
            common_prefix_len++;
        }

        minimize(common_prefix_len);

        RefPtr<Node> node;
        if (m_unchecked_nodes.size() == 0)
            node = m_root;
        else
            node = m_unchecked_nodes.last().child;

        auto remaining = str.substring_view(common_prefix_len);
        for (char const c : remaining) {
            VERIFY(node->children().at(c) == nullptr);

            auto child = Node::create();
            node->children().at(c) = child;
            m_unchecked_nodes.append(UncheckedNode { node, c, child });
            node = child;
        }
        node->set_as_terminal();

        bool fits = str.copy_characters_to_buffer(m_previous_word_buf, sizeof(m_previous_word_buf));
        // It's guaranteed that m_previous_word_buf is large enough to hold the longest named character reference
        VERIFY(fits);
        m_previous_word = StringView(m_previous_word_buf, str.length());
    }

    void minimize(size_t down_to)
    {
        if (m_unchecked_nodes.size() == 0)
            return;
        while (m_unchecked_nodes.size() > down_to) {
            auto unchecked_node = m_unchecked_nodes.take_last();
            auto child = unchecked_node.child.release_nonnull();
            auto it = m_minimized_nodes.find(child);
            if (it != m_minimized_nodes.end()) {
                unchecked_node.parent->children().at(unchecked_node.character) = *it;
            } else {
                m_minimized_nodes.set(child);
            }
        }
    }

    void calc_numbers()
    {
        m_root->calc_numbers();
    }

    Optional<size_t> get_unique_index(StringView str)
    {
        size_t index = 0;
        Node* node = m_root.ptr();

        for (char const c : str) {
            if (node->children().at(c) == nullptr)
                return {};
            for (int sibling_c = 0; sibling_c < 128; sibling_c++) {
                if (node->children().at(sibling_c) == nullptr)
                    continue;
                if (sibling_c < c) {
                    index += node->children().at(sibling_c)->number();
                }
            }
            node = node->children().at(c);
            if (node->is_terminal())
                index += 1;
        }

        return index;
    }

    NonnullRefPtr<Node> root()
    {
        return m_root;
    }

private:
    NonnullRefPtr<Node> m_root;
    Node::NodeTableType m_minimized_nodes;
    Vector<UncheckedNode> m_unchecked_nodes;
    char m_previous_word_buf[64];
    StringView m_previous_word = { m_previous_word_buf, 0 };
};

struct NodeData {
    u8 character;
    u8 number;
    bool end_of_word;
    u16 child_index;
    u8 children_len;
};

static u16 queue_children(const NonnullRefPtr<Node>& node, Vector<NonnullRefPtr<Node>>& queue, HashMap<Node*, u16>& child_indexes, u16 first_available_index)
{
    auto current_available_index = first_available_index;
    for (u8 c = 0; c < 128; c++) {
        if (node->children().at(c) == nullptr)
            continue;
        auto child = NonnullRefPtr(*node->children().at(c));

        if (!child_indexes.contains(child.ptr())) {
            auto child_num_children = child->num_direct_children();
            if (child_num_children > 0) {
                child_indexes.set(child, current_available_index);
                current_available_index += child_num_children;
            }
            queue.append(child);
        }
    }
    return current_available_index;
}

static u16 write_children_data(const NonnullRefPtr<Node>& node, Vector<NodeData>& node_data, Vector<NonnullRefPtr<Node>>& queue, HashMap<Node*, u16>& child_indexes, u16 first_available_index)
{
    auto current_available_index = first_available_index;
    u8 unique_index_tally = 0;
    for (u8 c = 0; c < 128; c++) {
        if (node->children().at(c) == nullptr)
            continue;
        auto child = NonnullRefPtr(*node->children().at(c));
        auto child_num_children = child->num_direct_children();

        if (!child_indexes.contains(child.ptr())) {
            if (child_num_children > 0) {
                child_indexes.set(child, current_available_index);
                current_available_index += child_num_children;
            }
            queue.append(child);
        }

        node_data.append({ c, unique_index_tally, child->is_terminal(), child_indexes.get(child).value_or(0), child_num_children });

        unique_index_tally += child->number();
    }
    return current_available_index;
}

// Does not include the root node
static void write_node_data(DafsaBuilder& dafsa_builder, Vector<NodeData>& node_data)
{
    Vector<NonnullRefPtr<Node>> queue;
    HashMap<Node*, u16> child_indexes;

    u16 first_available_index = 1;
    first_available_index = queue_children(dafsa_builder.root(), queue, child_indexes, first_available_index);

    while (queue.size() > 0) {
        auto node = queue.take_first();
        first_available_index = write_children_data(node, node_data, queue, child_indexes, first_available_index);
    }
}

ErrorOr<void> generate_implementation_file(JsonObject& named_character_reference_data, Core::File& file)
{
    StringBuilder builder;
    SourceGenerator generator { builder };
    DafsaBuilder dafsa_builder;

    named_character_reference_data.for_each_member([&](auto& key, auto&) {
        dafsa_builder.insert(key.bytes_as_string_view().substring_view(1));
    });
    dafsa_builder.minimize(0);
    dafsa_builder.calc_numbers();

    // As a sanity check, confirm that the minimal perfect hashing doesn't
    // have any collisions
    {
        HashTable<size_t> index_set;

        named_character_reference_data.for_each_member([&](auto& key, auto&) {
            auto index = dafsa_builder.get_unique_index(key.bytes_as_string_view().substring_view(1)).value();
            VERIFY(!index_set.contains(index));
            index_set.set(index);
        });
        VERIFY(named_character_reference_data.size() == index_set.size());
    }

    auto index_to_codepoints = MUST(FixedArray<Codepoints>::create(named_character_reference_data.size()));

    named_character_reference_data.for_each_member([&](auto& key, auto& value) {
        auto codepoints = value.as_object().get_array("codepoints"sv).value();
        auto unique_index = dafsa_builder.get_unique_index(key.bytes_as_string_view().substring_view(1)).value();
        auto array_index = unique_index - 1;
        u32 second_codepoint = 0;
        if (codepoints.size() == 2) {
            second_codepoint = codepoints[1].template as_integer<u32>();
        }
        index_to_codepoints[array_index] = Codepoints { codepoints[0].template as_integer<u32>(), second_codepoint };
    });

    generator.append(R"~~~(
#include <LibWeb/HTML/Parser/Entities.h>

namespace Web::HTML {
)~~~");

    Vector<NodeData> node_data;
    write_node_data(dafsa_builder, node_data);

    generator.append(R"~~~(

NamedCharacterReferenceNode g_named_character_reference_nodes[] = {
    { 0, false, 0, 0, 0 },
)~~~");

    for (auto data : node_data) {
        auto member_generator = generator.fork();
        member_generator.set("char", StringView(&data.character, 1));
        member_generator.set("end_of_word", MUST(String::formatted("{}", data.end_of_word)));
        member_generator.set("number", String::number(data.number));
        member_generator.set("child_index", String::number(data.child_index));
        member_generator.set("children_len", String::number(data.children_len));
        member_generator.append(R"~~~(    { '@char@', @end_of_word@, @number@, @child_index@, @children_len@ },
)~~~");
    }

    generator.append(R"~~~(};

FirstLayerNode g_named_character_reference_first_layer[] = {
)~~~");

    auto num_children = dafsa_builder.root()->num_direct_children();
    VERIFY(num_children == 52); // A-Z, a-z exactly
    u16 unique_index_tally = 0;
    u16 first_child_index = 1;
    for (u8 c = 0; c < 128; c++) {
        if (dafsa_builder.root()->children().at(c) == nullptr)
            continue;
        VERIFY(AK::is_ascii_alpha(c));
        auto child = dafsa_builder.root()->children().at(c);
        auto child_num_children = child->num_direct_children();

        auto member_generator = generator.fork();
        member_generator.set("number", String::number(unique_index_tally));
        member_generator.set("child_index", String::number(first_child_index));
        member_generator.set("children_len", String::number(child_num_children));
        member_generator.append(R"~~~(    { @number@, @child_index@, @children_len@ },
)~~~");

        unique_index_tally += child->number();
        first_child_index += child_num_children;
    }

    generator.append(R"~~~(};

} // namespace Web::HTML
)~~~");

    TRY(file.write_until_depleted(generator.as_string_view().bytes()));
    return {};
}
