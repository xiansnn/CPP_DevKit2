add_executable(test_debug_probe test_debug_probe.cpp)
target_link_libraries(test_debug_probe
            pico_stdlib 
            utilities_probe
            )


include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)



add_executable(test_async_debug_probe test_async_debug_probe.cpp)
target_include_directories(test_async_debug_probe PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(test_async_debug_probe
            pico_stdlib 
            FreeRTOS-Kernel-Heap4 
            utilities_probe
)