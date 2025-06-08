/*
 * Copyright (c) 2020, Andreas Kling <andreas@ladybird.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/StringView.h>
#include <LibWeb/HTML/Parser/Entities.h>
#include <LibWeb/HTML/Parser/NamedCharacterReferences.h>

namespace Web::HTML {

bool NamedCharacterReferenceMatcher::try_consume_ascii_char(u8 c)
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

}
