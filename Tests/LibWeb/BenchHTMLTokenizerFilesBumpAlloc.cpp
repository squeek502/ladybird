#include <LibCore/File.h>
#include <LibCore/Directory.h>
#include <AK/ByteString.h>
#include <AK/Format.h>
#include <LibWeb/HTML/Parser/HTMLTokenizer.h>
#include <AK/Vector.h>
#include <AK/Assertions.h>

void* g_arena_start = NULL;
void* g_arena_next = NULL;
void* g_arena_end = NULL;

static inline size_t alignBackward(size_t addr, size_t align) {
    return addr & ~(align - 1);
}

static inline size_t alignForward(size_t addr, size_t align) {
    return alignBackward(addr + (align - 1), align);
}

extern "C" {
void *__wrap_malloc(size_t size);
void *__real_malloc(size_t size);

void *__wrap_malloc(size_t size) {
    if (g_arena_next == NULL) {
        return __real_malloc(size);
    }
    void* ptr = (void*)alignForward((size_t)g_arena_next, __STDCPP_DEFAULT_NEW_ALIGNMENT__);
    g_arena_next = (void*)((size_t)ptr + size);
    if (g_arena_next > g_arena_end) return NULL;
    return ptr;
}

void *__wrap_realloc(void* _ptr, size_t size);
void *__real_realloc(void* ptr, size_t size);

void *__wrap_realloc(void* _ptr, size_t size) {
    if (g_arena_next == NULL) {
        return __real_realloc(_ptr, size);
    }
    void* ptr = (void*)alignForward((size_t)g_arena_next, __STDCPP_DEFAULT_NEW_ALIGNMENT__);
    g_arena_next = (void*)((size_t)ptr + size);
    if (g_arena_next > g_arena_end) return NULL;
    return ptr;
}

void __wrap_free(void* ptr);
void __real_free(void* ptr);

void __wrap_free(void* ptr) {
    (void)ptr;
}
}

void*  operator new  ( size_t count, const std::nothrow_t&) noexcept {
    if (g_arena_next == NULL) {
        return malloc(count);
    }
    void* ptr = (void*)alignForward((size_t)g_arena_next, __STDCPP_DEFAULT_NEW_ALIGNMENT__);
    g_arena_next = (void*)((size_t)ptr + count);
    VERIFY(g_arena_next <= g_arena_end);
    if (g_arena_next > g_arena_end) return NULL;
    return ptr;
}
void*  operator new[]( size_t count, const std::nothrow_t&) noexcept {
    if (g_arena_next == NULL) {
        return malloc(count);
    }
    void* ptr = (void*)alignForward((size_t)g_arena_next, __STDCPP_DEFAULT_NEW_ALIGNMENT__);
    g_arena_next = (void*)((size_t)ptr + count);
    VERIFY(g_arena_next <= g_arena_end);
    if (g_arena_next > g_arena_end) return NULL;
    return ptr;
}
void*  operator new  ( size_t count, std::align_val_t align, const std::nothrow_t&) noexcept {
    if (g_arena_next == NULL) {
        return malloc(count);
    }
    void* ptr = (void*)alignForward((size_t)g_arena_next, (size_t)align);
    g_arena_next = (void*)((size_t)ptr + count);
    VERIFY(g_arena_next <= g_arena_end);
    if (g_arena_next > g_arena_end) return NULL;
    return ptr;
}
void*  operator new[]( size_t count, std::align_val_t align, const std::nothrow_t&) noexcept {
    if (g_arena_next == NULL) {
        return malloc(count);
    }
    void* ptr = (void*)alignForward((size_t)g_arena_next, (size_t)align);
    g_arena_next = (void*)((size_t)ptr + count);
    VERIFY(g_arena_next <= g_arena_end);
    if (g_arena_next > g_arena_end) return NULL;
    return ptr;
}
void operator delete(void* p) noexcept {
    (void)p;
}
void operator delete(void* p, size_t size) noexcept {
    (void)p;
    (void)size;
}
void operator delete[](void* p) noexcept {
    (void)p;
}
void operator delete[](void* p, size_t size) noexcept {
    (void)p;
    (void)size;
}

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
    auto arena_size = (size_t)100 * 1024 * 1024;
    g_arena_next = malloc(arena_size);
    g_arena_start = g_arena_next;
    if (g_arena_next == NULL) return 1;
    g_arena_end = (void*)((size_t)g_arena_next + arena_size);

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
        g_arena_next = g_arena_start;
        return IterationDecision::Continue;
    }));

    return 0;
}
