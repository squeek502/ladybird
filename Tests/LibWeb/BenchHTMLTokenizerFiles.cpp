#include <LibCore/File.h>
#include <LibCore/Directory.h>
#include <AK/ByteString.h>
#include <AK/Format.h>
#include <LibWeb/HTML/Parser/HTMLTokenizer.h>
#include <AK/Vector.h>
#include <AK/Assertions.h>

using Tokenizer = Web::HTML::HTMLTokenizer;
using Token = Web::HTML::HTMLToken;

static Vector<Token> run_tokenizer(StringView input, Tokenizer::NamedCharacterReferencesImplementation impl)
{
    Vector<Token> tokens;
    tokens.ensure_capacity(input.length() / 10);
    Tokenizer tokenizer { input, "UTF-8"sv };
    tokenizer.set_named_character_reference_implementation(impl, false);
    while (true) {
        auto maybe_token = tokenizer.next_token();
        if (!maybe_token.has_value())
            break;
        tokens.append(maybe_token.release_value());
    }
    return tokens;
}

int main(int argc, char* argv[]) {
    Tokenizer::NamedCharacterReferencesImplementation impl = Tokenizer::NamedCharacterReferencesImplementation::Dafsa;

    if (argc > 1) {
        if (strcmp(argv[1], "dafsa") == 0) {
            impl = Tokenizer::NamedCharacterReferencesImplementation::Dafsa;
        } else if (strcmp(argv[1], "dafsa-binary-search") == 0) {
            impl = Tokenizer::NamedCharacterReferencesImplementation::DafsaBinarySearch;
        } else if (strcmp(argv[1], "gecko") == 0) {
            impl = Tokenizer::NamedCharacterReferencesImplementation::Gecko;
        } else if (strcmp(argv[1], "blink") == 0) {
            impl = Tokenizer::NamedCharacterReferencesImplementation::Blink;
        }
    }

    StringView impl_names[] = { "Dafsa"sv, "DafsaBinarySearch"sv, "Gecko"sv, "Blink"sv };
    dbgln("{}", impl_names[(size_t)impl]);

    MUST(Core::Directory::for_each_entry("files"sv, Core::DirIterator::SkipParentAndBaseDir, [&](Core::DirectoryEntry const& entry, Core::Directory const& parent) {
        auto file = MUST(parent.open(entry.name, Core::File::OpenMode::Read));
        auto file_size = MUST(file->size());
        auto content = MUST(ByteBuffer::create_uninitialized(file_size));
        MUST(file->read_until_filled(content.bytes()));
        ByteString file_contents { content.bytes() };
        auto tokens = run_tokenizer(file_contents, impl);
        return IterationDecision::Continue;
    }));

    return 0;
}
