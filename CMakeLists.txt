cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_GENERATOR "Ninja")

find_program(CLANG_18 clang-18)
find_program(CLANGXX_18 clang++-18)
if(CLANG_18 AND CLANGXX_18)
    set(CMAKE_C_COMPILER ${CLANG_18})
    set(CMAKE_CXX_COMPILER ${CLANGXX_18})
endif()

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

project(colorful_thread_safe_print)
enable_testing()
add_library(colorful_thread_safe_print SHARED colorful_thread_safe_print.cc)
target_include_directories(colorful_thread_safe_print PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(colorful_thread_safe_print PUBLIC cxx_std_17)

add_executable(colorful_thread_safe_print_example example/basic_example.cc)
target_compile_features(colorful_thread_safe_print_example PRIVATE cxx_std_17)
target_link_libraries(colorful_thread_safe_print_example PRIVATE colorful_thread_safe_print)

add_executable(mytest0 test/test0.cc)
target_compile_features(mytest0 PRIVATE cxx_std_17)
find_package(Threads REQUIRED)
if(APPLE)
    find_program(BREW_PROGRAM brew)
    if(BREW_PROGRAM)
        execute_process(
            COMMAND ${BREW_PROGRAM} --prefix googletest
            OUTPUT_VARIABLE HOMEBREW_GTEST_PREFIX
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        if(HOMEBREW_GTEST_PREFIX AND EXISTS "${HOMEBREW_GTEST_PREFIX}")
            list(PREPEND CMAKE_PREFIX_PATH "${HOMEBREW_GTEST_PREFIX}")
        endif()
    endif()

    foreach(GTEST_PREFIX /opt/homebrew/opt/googletest /usr/local/opt/googletest)
        if(EXISTS "${GTEST_PREFIX}")
            list(PREPEND CMAKE_PREFIX_PATH "${GTEST_PREFIX}")
        endif()
    endforeach()
endif()
find_package(GTest CONFIG REQUIRED)
target_link_libraries(mytest0 PRIVATE GTest::gtest colorful_thread_safe_print Threads::Threads)
add_test(NAME mytest0 COMMAND mytest0)
