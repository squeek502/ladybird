/*
 * Copyright (C) 2010 Google, Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "AK/ByteString.h"
#include "AK/Format.h"
#include "LibWeb/HTML/Parser/BlinkEntityTable.h"
#include <LibWeb/HTML/Parser/BlinkEntitySearch.h>

namespace Web::HTML {

HTMLEntitySearch::HTMLEntitySearch() : range_(HTMLEntityTable::AllEntries()) {}

static Optional<u16> entry_to_char(const HTMLEntityTableEntry& entry, u16 current_length) {
  if (entry.length < current_length + 1) {
    return {};
  }
  auto entity_string =
      HTMLEntityTable::EntityString(entry);
  return entity_string[current_length];
}

static bool optional_less_than(Optional<u16> a, Optional<u16> b) {
  if (a.has_value() && b.has_value()) {
    return a.value() < b.value();
  }
  return b.has_value() && !a.has_value();
}

static Span<HTMLEntityTableEntry const>::Iterator lower_bound(Span<HTMLEntityTableEntry const>::Iterator first, Span<HTMLEntityTableEntry const>::Iterator last, u16 current_length, u16 next_character) {
  ptrdiff_t len = last - first;

  while (len > 0) {
    auto half = len / 2;
    auto middle = first;
    middle += half;

    auto val = entry_to_char(*middle, current_length);
    if (optional_less_than(val, next_character)) {
      first = middle;
      ++first;
      len = len - half - 1;
    } else {
      len = half;
    }
  }

  return first;
}

static Span<HTMLEntityTableEntry const>::Iterator upper_bound(Span<HTMLEntityTableEntry const>::Iterator first, Span<HTMLEntityTableEntry const>::Iterator last, u16 current_length, u16 next_character) {
  ptrdiff_t len = last - first;

  while (len > 0) {
    auto half = len / 2;
    auto middle = first;
    middle += half;

    auto val = entry_to_char(*middle, current_length);
    if (optional_less_than(next_character, val)) {
      len = half;
    } else {
      first = middle;
      ++first;
      len = len - half - 1;
    }
  }

  return first;
}

static Span<const HTMLEntityTableEntry> equal_range(Span<HTMLEntityTableEntry const>& range, u16 current_length, u16 next_character) {
  ptrdiff_t len = range.end() - range.begin();
  auto first = range.begin();

  while (len > 0) {
    auto half = len / 2;
    auto middle = first;
    middle += half;

    auto val = entry_to_char(*middle, current_length);
    if (optional_less_than(val, next_character)) {
      first = middle;
      ++first;
      len = len - half - 1;
    } else if (optional_less_than(next_character, val)) {
      len = half;
    } else {
      auto left = lower_bound(first, middle, current_length, next_character);
      first += len;
      auto right = upper_bound(++middle, first, current_length, next_character);
      return range.slice(left.index(), right - left);
    }
  }

  return range.slice(first.index(), 0);
}

u64 g_blink_advance_call_count = 0;

void HTMLEntitySearch::Advance(u16 next_character) {
  g_blink_advance_call_count++;
  VERIFY(IsEntityPrefix());
  if (!current_length_) {
    range_ = HTMLEntityTable::EntriesStartingWith(next_character);
  } else {
    // Get the subrange where `next_character` matches at the end of the
    // current prefix (index == `current_length_`).

    // range_ = std::ranges::equal_range(range_, next_character, std::less{},
    //                                   projector);
    range_ = equal_range(range_, current_length_, next_character);
  }
  if (range_.is_empty()) {
    Fail();
    return;
  }
  ++current_length_;
  if (range_.first().length != current_length_) {
    return;
  }
  most_recent_match_ = &range_.first();
}

}  // namespace blink
