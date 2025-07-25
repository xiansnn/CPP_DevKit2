add_executable(test_async_debug_probe test_async_debug_probe.cpp)
target_include_directories(test_async_debug_probe PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(test_async_debug_probe
            pico_stdlib 
            FreeRTOS-Kernel-Heap4 
            utilities_probe
)