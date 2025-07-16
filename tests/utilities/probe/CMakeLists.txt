add_executable(test_debug_probe test_debug_probe.cpp)
target_link_libraries(test_debug_probe
            pico_stdlib 
            utilities_probe
            )

add_executable(test_async_debug_probe test_async_debug_probe.cpp)
target_link_libraries(test_async_debug_probe
            pico_stdlib 
            utilities_probe
)