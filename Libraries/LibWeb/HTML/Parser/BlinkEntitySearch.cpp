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

static Span<HTMLEntityTableEntry const>::Iterator lower_bound(Span<HTMLEntityTableEntry const>& range, u16 current_length, u16 next_character) {
  ptrdiff_t count = range.end() - range.begin();
  ptrdiff_t step;
  Span<HTMLEntityTableEntry const>::Iterator it;
  auto first = range.begin();

  while (count > 0) {
    it = first;
    step = count / 2;
    it += step;

    auto val = entry_to_char(*it, current_length);
    if (optional_less_than(val, next_character)) {
      first = ++it;
      count -= step + 1;
    } else {
      count = step;
    }
  }

  return first;
}

static Span<HTMLEntityTableEntry const>::Iterator upper_bound(Span<HTMLEntityTableEntry const>& range, u16 current_length, u16 next_character) {
  ptrdiff_t count = range.end() - range.begin();
  ptrdiff_t step;
  Span<HTMLEntityTableEntry const>::Iterator it;
  auto first = range.begin();

  while (count > 0) {
    it = first;
    step = count / 2;
    it += step;

    auto val = entry_to_char(*it, current_length);
    if (!optional_less_than(next_character, val)) {
      first = ++it;
      count -= step + 1;
    } else {
      count = step;
    }
  }

  return first;
}

void HTMLEntitySearch::Advance(u16 next_character) {
  VERIFY(IsEntityPrefix());
  if (!current_length_) {
    range_ = HTMLEntityTable::EntriesStartingWith(next_character);
  } else {
    // Get the subrange where `next_character` matches at the end of the
    // current prefix (index == `current_length_`).

    dbgln("{}: {}-{}:{}", next_character, range_.begin().index(), range_.end().index(), current_length_);

    // range_ = std::ranges::equal_range(range_, next_character, std::less{},
    //                                   projector);
    auto lower = lower_bound(range_, current_length_, next_character);
    auto upper = upper_bound(range_, current_length_, next_character);
    dbgln(" -> {}-{}", lower.index(), upper.index());
    range_ = range_.slice(lower.index(), upper.index() - lower.index());
    dbgln("range length: {}", range_.size());
    for (auto it = range_.begin(); it != range_.end(); ++it) {
      auto byte_string = ByteString((const char*)HTMLEntityTable::EntityString(*it).data(), HTMLEntityTable::EntityString(*it).size());
      dbgln("{}", byte_string);
    }
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
