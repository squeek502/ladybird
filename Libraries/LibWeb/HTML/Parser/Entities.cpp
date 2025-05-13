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

        auto absolute_index = match - g_named_character_reference_chars;
        m_pending_unique_index += g_named_character_reference_numbers[absolute_index].number;
        m_overconsumed_code_points++;

        if (match->end_of_word) {
            m_pending_unique_index += 1;
            m_last_matched_unique_index = m_pending_unique_index;
            m_ends_with_semicolon = c == ';';
            m_overconsumed_code_points = 0;
        }

        auto child_data = g_named_character_reference_children[absolute_index];
        m_children_to_check = ReadonlySpan<CharData>(&g_named_character_reference_chars[child_data.child_index], child_data.children_len);
        return true;
    } else {
        if (AK::is_ascii_alpha(c)) {
            auto index = c <= 'Z' ? c - 'A' : c - 'a' + 26;
            auto data = g_named_character_reference_first_layer[index];
            m_children_to_check = ReadonlySpan<CharData>(&g_named_character_reference_chars[data.child_index], data.children_len);
            m_overconsumed_code_points++;
            m_pending_unique_index = data.number;
            return true;
        }
        return false;
    }
}

}
