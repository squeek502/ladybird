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


# BenchMatcher (Zig port)
if(EXISTS "../../Libraries/LibWeb/HTML/Parser/BlinkEntityTable.cpp")
    # Blink
    add_executable(BenchMatcher BenchHTMLTokenizerBranch.cpp ../../Libraries/LibWeb/HTML/Parser/Entities.cpp ../../Libraries/LibWeb/HTML/Parser/BlinkEntitySearch.cpp ../../Libraries/LibWeb/HTML/Parser/BlinkEntityTable.cpp)
elseif(EXISTS "../../Libraries/LibWeb/HTML/Parser/GeckoNamedCharactersAccel.cpp")
    # Gecko
    add_executable(BenchMatcher BenchHTMLTokenizerBranch.cpp ../../Libraries/LibWeb/HTML/Parser/Entities.cpp ../../Libraries/LibWeb/HTML/Parser/GeckoNamedCharacters.cpp ../../Libraries/LibWeb/HTML/Parser/GeckoNamedCharactersAccel.cpp)
else()
    # Ladybird
    add_executable(BenchMatcher BenchHTMLTokenizerBranch.cpp ../../Libraries/LibWeb/HTML/Parser/Entities.cpp ../../Libraries/LibWeb/HTML/Parser/NamedCharacterReferences.cpp)
    set_source_files_properties(../../Libraries/LibWeb/HTML/Parser/NamedCharacterReferences.cpp PROPERTIES GENERATED 1)
    add_dependencies(BenchMatcher GenerateNamedCharacterReferences)
endif()
target_link_libraries(BenchMatcher PRIVATE AK)


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
