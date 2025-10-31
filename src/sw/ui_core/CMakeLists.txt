add_library(sw_ui_core ui_core.cpp)
target_link_libraries(sw_ui_core 
            pico_stdlib 
            )
# add_library(rtos_ui_core rtos_ui_core.cpp)
# target_link_libraries(rtos_ui_core 
#             pico_stdlib 
#             FreeRTOS-Kernel-Heap4
#             )
