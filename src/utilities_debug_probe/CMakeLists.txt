add_library(debug_probe probe.cpp)
# target_include_directories(debug_probe 
#     PUBLIC 
#     ${PROJECT_SOURCE_DIR}/include
#     )
target_link_libraries(debug_probe 
    pico_stdlib
    )