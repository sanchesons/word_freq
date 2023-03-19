cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_COMPILER g++)
project(word_freq VERSION 0.0.1 LANGUAGES CXX)

include_directories(
    src/
    src/stream/
)

add_library(utf_convertor STATIC
    src/stream/utf_convertor.cpp
)
target_link_libraries(utf_convertor
    icuuc
)
set_target_properties(utf_convertor
PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

############################# WF ######################################
add_executable(wf
    src/main.cpp
)
target_compile_options(wf PRIVATE
    -fconcepts
)
target_link_libraries(wf
    icuuc
    utf_convertor
)
set_target_properties(wf
PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

add_subdirectory(test)

add_subdirectory(banchmark)
