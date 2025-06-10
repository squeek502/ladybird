/*
 * Copyright (c) 2020, Andreas Kling <andreas@ladybird.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "AK/Assertions.h"
#include "AK/CharacterTypes.h"
#include <AK/StringView.h>
#include <LibWeb/HTML/Parser/Entities.h>
#include <LibWeb/HTML/Parser/NamedCharacterReferences.h>
#include <LibWeb/HTML/Parser/GeckoNamedCharacters.h>
#include <LibWeb/HTML/Parser/GeckoNamedCharactersAccel.h>

namespace Web::HTML {

bool NamedCharacterReferenceMatcher::try_consume_ascii_char(u8 c)
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
