/*
 * Copyright (c) 2020, Andreas Kling <andreas@ladybird.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Optional.h>
#include <AK/Types.h>
#include <LibWeb/HTML/Parser/NamedCharacterReferences.h>
#include <LibWeb/HTML/Parser/BlinkEntitySearch.h>

namespace Web::HTML {

static NamedCharacterReferenceSecondCodepoint get_second_codepoint_enum(u32 codepoint)
{
    switch (codepoint) {
    case 0x0338:
        return NamedCharacterReferenceSecondCodepoint::CombiningLongSolidusOverlay;
    case 0x20D2:
        return NamedCharacterReferenceSecondCodepoint::CombiningLongVerticalLineOverlay;
    case 0x200A:
        return NamedCharacterReferenceSecondCodepoint::HairSpace;
    case 0x0333:
        return NamedCharacterReferenceSecondCodepoint::CombiningDoubleLowLine;
    case 0x20E5:
        return NamedCharacterReferenceSecondCodepoint::CombiningReverseSolidusOverlay;
    case 0xFE00:
        return NamedCharacterReferenceSecondCodepoint::VariationSelector1;
    case 0x006A:
        return NamedCharacterReferenceSecondCodepoint::LatinSmallLetterJ;
    case 0x0331:
        return NamedCharacterReferenceSecondCodepoint::CombiningMacronBelow;
    default:
        return NamedCharacterReferenceSecondCodepoint::None;
    }
}

class NamedCharacterReferenceMatcher {
public:
    NamedCharacterReferenceMatcher() = default;

    // If `c` is the codepoint of a child of the current `node_index`, the `node_index`
    // is updated to that child and the function returns `true`.
    // Otherwise, the `node_index` is unchanged and the function returns false.
    bool try_consume_code_point(u32 c)
    {
        if (c > 0x7F)
            return false;
        return try_consume_ascii_char(static_cast<u8>(c));
    }

    // If `c` is the character of a child of the current `node_index`, the `node_index`
    // is updated to that child and the function returns `true`.
    // Otherwise, the `node_index` is unchanged and the function returns false.
    bool try_consume_ascii_char(u8 c);

    // Returns the code points associated with the last match, if any.
    Optional<NamedCharacterReferenceCodepoints> code_points() const {
        if (m_search.MostRecentMatch() != NULL) {
            const auto *match = m_search.MostRecentMatch();
            return NamedCharacterReferenceCodepoints{ match->first_value, get_second_codepoint_enum(match->second_value) };
        }
        return {};
    }

    bool last_match_ends_with_semicolon() const { return m_ends_with_semicolon; }

    u8 overconsumed_code_points() const { return m_overconsumed_code_points; }

private:
    HTMLEntitySearch m_search;
    u8 m_overconsumed_code_points { 0 };
    bool m_ends_with_semicolon { false };
};

}
