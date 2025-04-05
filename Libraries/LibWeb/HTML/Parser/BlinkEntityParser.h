/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_HTML_PARSER_HTML_ENTITY_PARSER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_HTML_PARSER_HTML_ENTITY_PARSER_H_

#include <AK/Assertions.h>
#include <AK/Types.h>
#include <AK/Array.h>

namespace Web::HTML {

class DecodedHTMLEntity {
 private:
  // HTML entities contain at most 2 Unicode code points.
  static const unsigned kMaxLength = 2;

 public:
  bool IsEmpty() const { return !length; }

  void Append(u32 c) {
    VERIFY(length < kMaxLength);
    data[length++] = c;
  }

  unsigned length = 0;
  Array<u32, kMaxLength> data;
};

void AppendLegalEntityFor(u32 c, DecodedHTMLEntity& decoded_entity);

bool ConsumeHTMLEntity(StringView,
                                   DecodedHTMLEntity& decoded_entity,
                                   bool at_eof,
                                   bool& not_enough_characters,
                                   size_t& entity_length,
                                   size_t& overconsumed_characters,
                                   u16 additional_allowed_character = '\0');

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_HTML_PARSER_HTML_ENTITY_PARSER_H_
