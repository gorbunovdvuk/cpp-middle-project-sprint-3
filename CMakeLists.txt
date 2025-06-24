cmake_minimum_required(VERSION 3.30)
project(BookDB VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wfatal-errors)
endif ()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
string(APPEND CMAKE_CXX_FLAGS_DEBUG " -g")
string(APPEND CMAKE_CXX_FLAGS_RELEASE " -O3 -march=native")

find_package(GTest REQUIRED)
find_package(magic_enum REQUIRED)
find_package(Boost REQUIRED COMPONENTS container)

file(GLOB BOOKDB_HEADERS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/include/*.hpp")

add_library(bookdb STATIC "${CMAKE_SOURCE_DIR}/src/main.cpp" ${BOOKDB_HEADERS})
target_include_directories(bookdb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(bookdb PUBLIC magic_enum::magic_enum Boost::container)

add_executable(bookdb_cli src/main.cpp)
target_link_libraries(bookdb_cli PRIVATE bookdb)

find_package(benchmark REQUIRED)
add_executable(bookdb_benchmark src/benchmark.cpp)
target_link_libraries(bookdb_benchmark PRIVATE bookdb benchmark::benchmark)

file(GLOB BOOKDB_TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/tests/*.cpp")
add_executable(bookdb_tests ${BOOKDB_TEST_SOURCES})
target_link_libraries(bookdb_tests PRIVATE bookdb GTest::GTest)

enable_testing()
add_test(NAME BookDB_Tests COMMAND bookdb_tests)
