cmake_minimum_required(VERSION 3.28)
project(TradeMatchingEngine CXX)

# Use C++20
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

# Create the executable
add_executable(TradeMatchingEngine
    ${SRC_DIR}/main.cpp
)

target_include_directories(TradeMatchingEngine PRIVATE ${INCLUDE_DIR})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(TradeMatchingEngine PRIVATE -Wall -Wextra -Wpedantic -Werror)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(TradeMatchingEngine PRIVATE /W4 /WX)
endif()
