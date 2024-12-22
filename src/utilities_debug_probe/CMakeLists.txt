add_library(debug_probe probe.cpp)
target_link_libraries(debug_probe 
    pico_stdlib
    )