/*
 * Copyright (c) 2020, Andreas Kling <andreas@ladybird.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Optional.h>
#include <AK/Types.h>
#include <LibWeb/HTML/Parser/NamedCharacterReferences.h>
#include <LibWeb/HTML/Parser/GeckoNamedCharacters.h>

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

enum NamedCharacterReferenceMatcherState : u8 {
    Init,
    HiLoLookup,
    Tail,
};

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
        if (m_candidate == -1) return {};

        const char16_t* val = nsHtml5NamedCharacters::VALUES[m_candidate];
        // Stored as UTF-16 code units, so we need to decode the code point
        // if the first code unit is a high surrogate
        if (val[0] >= 0xD800 && val[0] <= 0xDBFF) {
            u32 code_point = (0x10000 + ((val[0] & 0x03ff) << 10)) | (val[1] & 0x03ff);
            return NamedCharacterReferenceCodepoints{ code_point, NamedCharacterReferenceSecondCodepoint::None };
        }
        return NamedCharacterReferenceCodepoints{ val[0], get_second_codepoint_enum(val[1]) };
    }

    bool last_match_ends_with_semicolon() const { return m_ends_with_semicolon; }

    u8 overconsumed_code_points() const { return m_overconsumed_code_points; }

private:
    NamedCharacterReferenceMatcherState m_state { NamedCharacterReferenceMatcherState::Init };
    int32_t m_ent_col { -1 };
    int32_t m_lo { 0 };
    int32_t m_hi { 0 };
    int32_t m_candidate { -1 };
    u8 m_overconsumed_code_points { 0 };
    bool m_ends_with_semicolon { false };
};

}
