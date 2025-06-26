set(TEST_SOURCES
    TestCSSIDSpeed.cpp
    TestCSSPixels.cpp
    TestCSSTokenStream.cpp
    TestCSSInheritedProperty.cpp
    TestFetchInfrastructure.cpp
    TestFetchURL.cpp
    TestHTMLTokenizer.cpp
    TestMicrosyntax.cpp
    TestMimeSniff.cpp
    TestNumbers.cpp
)

foreach(source IN LISTS TEST_SOURCES)
    serenity_test("${source}" LibWeb LIBS LibWeb)
endforeach()

# BenchHTMLTokenizer (Zig port)
add_executable(BenchMatcher
    BenchMatcher.cpp
    ../../Libraries/LibWeb/HTML/Parser/Entities.cpp
    ../../Libraries/LibWeb/HTML/Parser/NamedCharacterReferences.cpp
    ../../Libraries/LibWeb/HTML/Parser/NamedCharacterReferencesBinarySearch.cpp
    ../../Libraries/LibWeb/HTML/Parser/GeckoNamedCharacters.cpp
    ../../Libraries/LibWeb/HTML/Parser/GeckoNamedCharactersAccel.cpp
    ../../Libraries/LibWeb/HTML/Parser/BlinkEntitySearch.cpp
    ../../Libraries/LibWeb/HTML/Parser/BlinkEntityTable.cpp
)
set_source_files_properties(../../Libraries/LibWeb/HTML/Parser/NamedCharacterReferencesBinarySearch.cpp PROPERTIES GENERATED 1)
set_source_files_properties(../../Libraries/LibWeb/HTML/Parser/NamedCharacterReferences.cpp PROPERTIES GENERATED 1)
add_dependencies(BenchMatcher GenerateNamedCharacterReferences GenerateNamedCharacterReferencesBinarySearch)
target_link_libraries(BenchMatcher PRIVATE AK)


# BenchHTMLTokenizer
add_executable(BenchHTMLTokenizer BenchHTMLTokenizer.cpp)
target_link_libraries(BenchHTMLTokenizer PRIVATE AK LibWeb)


# BenchHTMLTokenizerFiles
add_executable(BenchHTMLTokenizerFiles BenchHTMLTokenizerFiles.cpp)
target_link_libraries(BenchHTMLTokenizerFiles PRIVATE AK LibWeb LibCore)

# BenchHTMLTokenizerFilesBumpAlloc
add_executable(BenchHTMLTokenizerFilesBumpAlloc BenchHTMLTokenizerFilesBumpAlloc.cpp)
target_link_libraries(BenchHTMLTokenizerFilesBumpAlloc PRIVATE AK LibWeb LibCore)
target_link_options(BenchHTMLTokenizerFilesBumpAlloc PRIVATE -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=realloc)


target_link_libraries(TestFetchURL PRIVATE LibURL)

if (ENABLE_SWIFT)
    find_package(SwiftTesting REQUIRED)

    add_executable(TestLibWebSwift
        TestLibWebSwiftBindings.swift
        TestHTMLTokenizerSwift.swift
    )

    # FIXME: Swift doesn't seem to like object libraries for @main
    target_sources(TestLibWebSwift PRIVATE ../Resources/SwiftTestMain.swift)

    set_target_properties(TestLibWebSwift PROPERTIES SUFFIX .swift-testing)
    target_link_libraries(TestLibWebSwift PRIVATE AK LibWeb SwiftTesting::SwiftTesting)
    add_test(NAME TestLibWebSwift COMMAND TestLibWebSwift)
endif()
