/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 * Copyright (C) 2009 Torch Mobile, Inc. http://www.torchmobile.com/
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

#include "AK/CharacterTypes.h"
#include <LibWeb/HTML/Parser/BlinkEntityParser.h>

#include <LibWeb/HTML/Parser/BlinkEntitySearch.h>
#include <LibWeb/HTML/Parser/BlinkEntityTable.h>
#include <AK/StringView.h>
#include <AK/Vector.h>

namespace Web::HTML {

namespace {

void AppendMatchToDecoded(const HTMLEntityTableEntry& match,
                          DecodedHTMLEntity& decoded_entity) {
  decoded_entity.Append(match.first_value);
  if (match.second_value) {
    decoded_entity.Append(match.second_value);
  }
}

typedef Vector<u16, 64> ConsumedCharacterBuffer;

bool ConsumeNamedEntity(StringView source,
                        DecodedHTMLEntity& decoded_entity,
                        bool at_eof,
                        bool& not_enough_characters,
                        u16 additional_allowed_character,
                        u16& cc,
                        size_t& entity_length,
                        size_t& overconsumed_characters) {
  ConsumedCharacterBuffer consumed_characters;
  HTMLEntitySearch entity_search;
  size_t cc_i = 0;
  while (cc_i < source.length()) {
    cc = (u8)source[cc_i];
    entity_search.Advance(cc);
    if (!entity_search.IsEntityPrefix())
      break;
    consumed_characters.append(cc);
    cc_i++; // source.AdvanceAndASSERT(cc);
  }
  // Character reference ends in ';', so if the last character is ';' then
  // don't treat it as not enough characters (because no additional characters
  // will change the result).
  not_enough_characters = !at_eof && cc_i == source.length() && cc != u';';
  if (not_enough_characters) {
    // We can't decide on an entity because there might be a longer entity
    // that we could match if we had more data.
    overconsumed_characters = consumed_characters.size(); // UnconsumeCharacters(source, consumed_characters);
    return false;
  }
  if (!entity_search.MostRecentMatch()) {
    overconsumed_characters = consumed_characters.size(); // UnconsumeCharacters(source, consumed_characters);
    return false;
  }
  if (entity_search.MostRecentMatch()->length !=
      entity_search.CurrentLength()) {
    // We've consumed too many characters. We need to walk the
    // source back to the point at which we had consumed an
    // actual entity.
    overconsumed_characters = consumed_characters.size() - entity_search.MostRecentMatch()->length; // UnconsumeCharacters(source, consumed_characters);
    consumed_characters.clear();
    const HTMLEntityTableEntry* most_recent = entity_search.MostRecentMatch();
    const auto reference =
        HTMLEntityTable::EntityString(*most_recent);
    size_t i;
    for (i = 0; i < reference.size(); ++i) {
      cc = (u8)source[i];
      VERIFY(cc == reference[i]);
      consumed_characters.append(cc);
      // source.AdvanceAndASSERT(cc);
      VERIFY(i < source.length());
    }
    cc = (u8)source[i];
  }
  if (entity_search.MostRecentMatch()->LastCharacter() == ';' ||
      !additional_allowed_character ||
      !(is_ascii_alphanumeric(cc) || cc == '=')) {
    entity_length = entity_search.MostRecentMatch()->length;
    AppendMatchToDecoded(*entity_search.MostRecentMatch(), decoded_entity);
    return true;
  }
  overconsumed_characters = consumed_characters.size(); // UnconsumeCharacters(source, consumed_characters);
  return false;
}

}  // namespace

void AppendLegalEntityFor(u32  /*c*/, DecodedHTMLEntity&  /*decoded_entity*/) {
  VERIFY_NOT_REACHED();
}

bool ConsumeHTMLEntity(StringView source,
                       DecodedHTMLEntity& decoded_entity,
                       bool at_eof,
                       bool& not_enough_characters,
                       size_t& entity_length,
                       size_t& overconsumed_characters,
                       u16 additional_allowed_character) {
  VERIFY(!additional_allowed_character || additional_allowed_character == '"' ||
         additional_allowed_character == '\'' ||
         additional_allowed_character == '>');
  VERIFY(!not_enough_characters);
  VERIFY(decoded_entity.IsEmpty());

  enum EntityState {
    kInitial,
    kNumber,
    kMaybeHexLowerCaseX,
    kMaybeHexUpperCaseX,
    kHex,
    kDecimal,
    kNamed
  };
  EntityState entity_state = kInitial;
  ConsumedCharacterBuffer consumed_characters;

  size_t i = 0;
  while (source.length()) {
    u16 cc = (u8)source[i];
    switch (entity_state) {
      case kInitial: {
        if (cc == '\x09' || cc == '\x0A' || cc == '\x0C' || cc == ' ' ||
            cc == '<' || cc == '&')
          return false;
        if (additional_allowed_character && cc == additional_allowed_character)
          return false;
        if (cc == '#') {
          entity_state = kNumber;
          break;
        }
        if ((cc >= 'a' && cc <= 'z') || (cc >= 'A' && cc <= 'Z')) {
          entity_state = kNamed;
          continue;
        }
        return false;
      }
      case kNumber: {
        VERIFY_NOT_REACHED();
      }
      case kMaybeHexLowerCaseX: {
        VERIFY_NOT_REACHED();
      }
      case kMaybeHexUpperCaseX: {
        VERIFY_NOT_REACHED();
      }
      case kHex: {
        VERIFY_NOT_REACHED();
      }
      case kDecimal: {
        VERIFY_NOT_REACHED();
      }
      case kNamed: {
        return ConsumeNamedEntity(source, decoded_entity, at_eof, not_enough_characters,
                                  additional_allowed_character, cc,
                                  entity_length, overconsumed_characters);
      }
    }

    consumed_characters.append(cc);
    i++; //source.AdvanceAndASSERT(cc);
  }
  VERIFY(i < source.length());
  not_enough_characters = true;
  overconsumed_characters = consumed_characters.size(); // UnconsumeCharacters(source, consumed_characters);
  return false;
}

}  // namespace blink
