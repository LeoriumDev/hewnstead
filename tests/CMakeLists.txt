add_executable(hewnstead_tests
    test_main.cpp
    test_chunk.cpp
    test_chunk_coord.cpp
    test_chunk_manager.cpp
)

target_link_libraries(hewnstead_tests PRIVATE
    hewnstead_core
    doctest::doctest
)

target_compile_features(hewnstead_tests PRIVATE cxx_std_20)

add_test(NAME hewnstead_tests COMMAND hewnstead_tests)
