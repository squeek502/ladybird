/*
 * Copyright (c) 2020, Andreas Kling <andreas@ladybird.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "AK/Format.h"
#include <AK/Span.h>
#include <AK/Optional.h>
#include <AK/Types.h>
#include <LibWeb/HTML/Parser/NamedCharacterReferences.h>
#include <LibWeb/HTML/Parser/NamedCharacterReferencesBinarySearch.h>
#include <LibWeb/HTML/Parser/GeckoNamedCharacters.h>
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
    virtual ~NamedCharacterReferenceMatcher() = default;

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
    virtual bool try_consume_ascii_char(u8 c) = 0;

    // Returns the code points associated with the last match, if any.
    virtual Optional<NamedCharacterReferenceCodepoints> code_points() const = 0;

    virtual StringView name() const = 0;

    virtual void reset() {
        m_overconsumed_code_points = 0;
        m_ends_with_semicolon = false;
    }

    bool last_match_ends_with_semicolon() const { return m_ends_with_semicolon; }

    u8 overconsumed_code_points() const { return m_overconsumed_code_points; }

protected:
    u8 m_overconsumed_code_points { 0 };
    bool m_ends_with_semicolon { false };
};

class NamedCharacterReferenceMatcherDafsa : public NamedCharacterReferenceMatcher {
public:
    NamedCharacterReferenceMatcherDafsa() = default;

    Optional<NamedCharacterReferenceCodepoints> code_points() const override {
        return named_character_reference_codepoints_from_unique_index(m_last_matched_unique_index);
    }

    bool try_consume_ascii_char(u8 c) override;

    StringView name() const override { return "Dafsa"sv; }

    void reset() override {
        m_node_index = 0;
        m_last_matched_unique_index = 0;
        m_pending_unique_index = 0;
        NamedCharacterReferenceMatcher::reset();
    }

    // Returns true if the current `node_index` is marked as the end of a word
    bool currently_matches() const { return named_character_reference_is_end_of_word(m_node_index); }

private:
    u16 m_node_index { 0 };
    u16 m_last_matched_unique_index { 0 };
    u16 m_pending_unique_index { 0 };
    int64_t m_padding1 { 0 };
    int64_t m_padding2 { 0 };
    int64_t m_padding3 { 0 };
    int64_t m_padding4 { 0 };
};

class NamedCharacterReferenceMatcherDafsaBinarySearch : public NamedCharacterReferenceMatcher {
public:
    NamedCharacterReferenceMatcherDafsaBinarySearch() = default;

    Optional<NamedCharacterReferenceCodepoints> code_points() const override {
        return named_character_reference_codepoints_from_unique_index(m_last_matched_unique_index);
    }

    bool try_consume_ascii_char(u8 c) override;

    StringView name() const override { return "DafsaBinarySearch"sv; }

    void reset() override {
        m_children_to_check = {};
        m_last_matched_unique_index = 0;
        m_pending_unique_index = 0;
        NamedCharacterReferenceMatcher::reset();
    }

private:
    Optional<ReadonlySpan<NamedCharacterReferenceNode>> m_children_to_check;
    u16 m_last_matched_unique_index { 0 };
    u16 m_pending_unique_index { 0 };
};

class NamedCharacterReferenceMatcherBlink : public NamedCharacterReferenceMatcher {
public:
    NamedCharacterReferenceMatcherBlink() = default;

    Optional<NamedCharacterReferenceCodepoints> code_points() const override {
        if (m_search.MostRecentMatch() != NULL) {
            const auto *match = m_search.MostRecentMatch();
            return NamedCharacterReferenceCodepoints{ match->first_value, get_second_codepoint_enum(match->second_value) };
        }
        return {};
    }

    bool try_consume_ascii_char(u8 c) override;

    StringView name() const override { return "Blink"sv; }

    void reset() override {
        m_search = {};
        NamedCharacterReferenceMatcher::reset();
    }

private:
    HTMLEntitySearch m_search;
};


class NamedCharacterReferenceMatcherGecko : public NamedCharacterReferenceMatcher {
public:
    NamedCharacterReferenceMatcherGecko() = default;

    enum NamedCharacterReferenceMatcherState : u8 {
        Init,
        HiLoLookup,
        Tail,
    };

    Optional<NamedCharacterReferenceCodepoints> code_points() const override {
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

    bool try_consume_ascii_char(u8 c) override;

    StringView name() const override { return "Blink"sv; }

    void reset() override {
        m_state = NamedCharacterReferenceMatcherState::Init;
        m_ent_col = -1;
        m_lo = 0;
        m_hi = 0;
        m_candidate = -1;
        NamedCharacterReferenceMatcher::reset();
    }

private:
    NamedCharacterReferenceMatcherState m_state { NamedCharacterReferenceMatcherState::Init };
    int32_t m_ent_col { -1 };
    int32_t m_lo { 0 };
    int32_t m_hi { 0 };
    int32_t m_candidate { -1 };
    int64_t m_padding1 { 0 };
    int64_t m_padding2 { 0 };
};

}
