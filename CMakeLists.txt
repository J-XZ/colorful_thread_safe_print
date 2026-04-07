cmake_minimum_required(VERSION 3.10)
include(CheckIPOSupported)

if(DEFINED CXLKV_ENABLE_ASAN AND NOT DEFINED CTSP_ENABLE_ASAN)
    set(CTSP_ENABLE_ASAN "${CXLKV_ENABLE_ASAN}" CACHE BOOL
        "Enable AddressSanitizer for colorful_thread_safe_print" FORCE)
else()
    option(CTSP_ENABLE_ASAN "Enable AddressSanitizer for colorful_thread_safe_print" OFF)
endif()

set(CMAKE_C_COMPILER clang-18)
set(CMAKE_CXX_COMPILER clang++-18)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_GENERATOR "Ninja")

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

project(colorful_thread_safe_print)
set(BUILD_SHARED_LIBS OFF)

if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU|AppleClang")
    string(APPEND CMAKE_C_FLAGS_RELEASE " -O3 -march=native")
    string(APPEND CMAKE_C_FLAGS_RELWITHDEBINFO " -O3 -g3 -march=native")
    string(APPEND CMAKE_C_FLAGS_DEBUG " -O0 -g3")
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU|AppleClang")
    string(APPEND CMAKE_CXX_FLAGS_RELEASE " -O3 -march=native")
    string(APPEND CMAKE_CXX_FLAGS_RELWITHDEBINFO " -O3 -g3 -march=native")
    string(APPEND CMAKE_CXX_FLAGS_DEBUG " -O0 -g3")
endif()

# Normalize ASAN flags first so ON/OFF switching in the same build dir does
# not keep stale sanitize instrumentation.
string(REPLACE "-fsanitize=address" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
string(REPLACE "-fno-omit-frame-pointer" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
string(REPLACE "-fsanitize=address" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REPLACE "-fno-omit-frame-pointer" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REPLACE "-fsanitize=address" "" CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
string(REPLACE "-fsanitize=address" "" CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")
string(REPLACE "-fsanitize=address" "" CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS}")
string(STRIP "${CMAKE_C_FLAGS}" CMAKE_C_FLAGS)
string(STRIP "${CMAKE_CXX_FLAGS}" CMAKE_CXX_FLAGS)
string(STRIP "${CMAKE_EXE_LINKER_FLAGS}" CMAKE_EXE_LINKER_FLAGS)
string(STRIP "${CMAKE_SHARED_LINKER_FLAGS}" CMAKE_SHARED_LINKER_FLAGS)
string(STRIP "${CMAKE_MODULE_LINKER_FLAGS}" CMAKE_MODULE_LINKER_FLAGS)

if(CTSP_ENABLE_ASAN)
    string(APPEND CMAKE_C_FLAGS " -fsanitize=address -fno-omit-frame-pointer")
    string(APPEND CMAKE_CXX_FLAGS " -fsanitize=address -fno-omit-frame-pointer")
    string(APPEND CMAKE_EXE_LINKER_FLAGS " -fsanitize=address")
    string(APPEND CMAKE_SHARED_LINKER_FLAGS " -fsanitize=address")
    string(APPEND CMAKE_MODULE_LINKER_FLAGS " -fsanitize=address")
endif()

check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
if(NOT ipo_supported)
    message(FATAL_ERROR "IPO/LTO is not supported: ${ipo_error}")
endif()

enable_testing()
add_library(colorful_thread_safe_print STATIC colorful_thread_safe_print.cc)
target_compile_options(colorful_thread_safe_print PRIVATE
    $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:-flto=full>
)
target_link_options(colorful_thread_safe_print PRIVATE
    $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:-flto=full>
)
target_include_directories(colorful_thread_safe_print PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(colorful_thread_safe_print PUBLIC cxx_std_17)

add_executable(colorful_thread_safe_print_example example/basic_example.cc)
target_compile_options(colorful_thread_safe_print_example PRIVATE
    $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:-flto=full>
)
target_link_options(colorful_thread_safe_print_example PRIVATE
    $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:-flto=full>
)
target_compile_features(colorful_thread_safe_print_example PRIVATE cxx_std_17)
target_link_libraries(colorful_thread_safe_print_example PRIVATE colorful_thread_safe_print)

add_executable(mytest0 test/test0.cc)
target_compile_options(mytest0 PRIVATE
    $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:-flto=full>
)
target_link_options(mytest0 PRIVATE
    $<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:-flto=full>
)
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
