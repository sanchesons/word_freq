add_executable(banchmark
    src/banchmark.cpp
)
target_compile_options(banchmark PRIVATE
    -fconcepts
)
target_link_libraries(banchmark
    icuuc
    utf_convertor
)
set_target_properties(banchmark PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
