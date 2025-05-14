/*
 * Copyright (c) 2020, Andreas Kling <andreas@ladybird.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/CharacterTypes.h>
#include <AK/StringView.h>
#include <AK/BinarySearch.h>
#include <LibWeb/HTML/Parser/Entities.h>
#include <LibWeb/HTML/Parser/NamedCharacterReferences.h>

namespace Web::HTML {

bool NamedCharacterReferenceMatcher::try_consume_ascii_char(u8 c)
{
    if (m_children_to_check.has_value()) {
        auto* match = binary_search(
            m_children_to_check.value(),
            c,
            nullptr,
            [](auto& c, auto& data) -> int {
                return (int)c - (int)data.character;
            });
        if (!match) return false;

        m_pending_unique_index += match->number;
        m_overconsumed_code_points++;

        if (match->end_of_word) {
            m_pending_unique_index += 1;
            m_last_matched_unique_index = m_pending_unique_index;
            m_ends_with_semicolon = c == ';';
            m_overconsumed_code_points = 0;
        }

        m_children_to_check = ReadonlySpan<NamedCharacterReferenceNode>(&g_named_character_reference_nodes[match->child_index], match->children_len);
        return true;
    } else {
        if (AK::is_ascii_alpha(c)) {
            auto index = c <= 'Z' ? c - 'A' : c - 'a' + 26;
            auto data = g_named_character_reference_first_layer[index];
            m_children_to_check = ReadonlySpan<NamedCharacterReferenceNode>(&g_named_character_reference_nodes[data.child_index], data.children_len);
            m_overconsumed_code_points++;
            m_pending_unique_index = data.number;
            return true;
        }
        return false;
    }
}

}
