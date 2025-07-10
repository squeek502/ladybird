
#pragma once

#include <AK/Optional.h>
#include <AK/Types.h>
#include <LibWeb/HTML/Parser/NamedCharacterReferences.h>

namespace Web::HTML {

u16 named_character_reference_child_index(u16 node_index);
bool named_character_reference_is_end_of_word(u16 node_index);
Optional<u16> named_character_reference_find_sibling_and_update_unique_index(u16 first_child_index, u8 character, u16& unique_index);

} // namespace Web::HTML

