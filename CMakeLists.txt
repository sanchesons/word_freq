cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_COMPILER g++)
project(word_freq VERSION 0.0.1 LANGUAGES CXX)

include_directories(
    src/
)

add_executable(wf
    src/main.cpp
)
target_link_libraries(wf
    icuuc
)
set_target_properties(wf 
PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

add_executable(test
    src/test.cpp
)
target_link_libraries(test
    icuuc
)
set_target_properties(test PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

add_executable(banchmark
    src/banchmark.cpp
)
target_link_libraries(banchmark
    icuuc
)
target_compile_options(banchmark PRIVATE
    -fconcepts
)
set_target_properties(banchmark PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)