/*
 * Copyright (c) 2020, Andreas Kling <andreas@ladybird.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "AK/Assertions.h"
#include <AK/BinarySearch.h>
#include <AK/CharacterTypes.h>
#include <AK/StringView.h>
#include <LibWeb/HTML/Parser/Entities.h>
#include <LibWeb/HTML/Parser/GeckoNamedCharactersAccel.h>

namespace Web::HTML {

bool NamedCharacterReferenceMatcherDafsa::try_consume_ascii_char(u8 c)
{
    auto child_index = named_character_reference_child_index(m_node_index);
    auto maybe_updated_index = named_character_reference_find_sibling_and_update_unique_index(child_index, c, m_pending_unique_index);
    if (!maybe_updated_index.has_value())
        return false;
    m_overconsumed_code_points++;
    m_node_index = maybe_updated_index.value();
    if (currently_matches()) {
        m_last_matched_unique_index = m_pending_unique_index;
        m_ends_with_semicolon = c == ';';
        m_overconsumed_code_points = 0;
    }
    return true;
}

bool NamedCharacterReferenceMatcherDafsaBinarySearch::try_consume_ascii_char(u8 c)
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

bool NamedCharacterReferenceMatcherBlink::try_consume_ascii_char(u8 c)
{
    m_search.Advance(c);
    if (!m_search.IsEntityPrefix())
        return false;

    m_overconsumed_code_points++;

    if (m_search.MostRecentMatch() && m_search.MostRecentMatch()->length == m_search.CurrentLength()) {
        m_ends_with_semicolon = c == ';';
        m_overconsumed_code_points = 0;
    }
    return true;
}

bool NamedCharacterReferenceMatcherGecko::try_consume_ascii_char(u8 c)
{
    switch (m_state) {
    case NamedCharacterReferenceMatcherState::Init:
    {
        if (!AK::is_ascii_alpha(c)) return false;
        // re-use lo as the first character
        m_lo = c;
        m_overconsumed_code_points++;
        m_state = NamedCharacterReferenceMatcherState::HiLoLookup;
        return true;
    }
    case NamedCharacterReferenceMatcherState::HiLoLookup:
    {
        if (c > 'z') return false;
        i32 const* row = nsHtml5NamedCharactersAccel::HILO_ACCEL[c];
        if (!row) return false;
        auto first_char = m_lo;
        size_t index;
        if (is_ascii_lower_alpha(first_char))
            index = first_char - 'a' + 26;
        else
            index = first_char - 'A';
        i32 hilo = row[index];
        if (!hilo) return false;
        m_overconsumed_code_points++;
        m_lo = hilo & 0xFFFF;
        m_hi = hilo >> 16;
        m_ent_col = -1;
        m_candidate = -1;
        if (nsHtml5NamedCharacters::NAMES[m_lo].length() == 0) {
            m_candidate = m_lo;
            m_overconsumed_code_points = 0;
            m_ends_with_semicolon = false;
        }
        m_state = NamedCharacterReferenceMatcherState::Tail;
        return true;
    }
    case NamedCharacterReferenceMatcherState::Tail:
    {
        m_ent_col++;
        auto ent_len = m_ent_col + 1;
        while (m_hi >= m_lo && (ent_len > nsHtml5NamedCharacters::NAMES[m_lo].length() || c > nsHtml5NamedCharacters::NAMES[m_lo].charAt(m_ent_col)))
            m_lo++;
        while (m_hi >= m_lo && (ent_len > nsHtml5NamedCharacters::NAMES[m_hi].length() || c < nsHtml5NamedCharacters::NAMES[m_hi].charAt(m_ent_col)))
            m_hi--;

        if (m_hi < m_lo)
            return false;

        m_overconsumed_code_points++;

        if (ent_len == nsHtml5NamedCharacters::NAMES[m_lo].length()) {
            m_candidate = m_lo;
            m_overconsumed_code_points = 0;
            m_ends_with_semicolon = c == ';';
        }
        return true;
    }
    default:
        VERIFY_NOT_REACHED();
    }

    VERIFY_NOT_REACHED();
}

}
