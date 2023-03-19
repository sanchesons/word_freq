add_executable(test
    src/test.cpp
)
target_compile_options(test PRIVATE
    -fconcepts
)
target_link_libraries(test
    icuuc
    utf_convertor
)
set_target_properties(test PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
