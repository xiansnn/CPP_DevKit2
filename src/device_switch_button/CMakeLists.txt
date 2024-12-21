add_library(switch_button switch_button.cpp)
target_include_directories(switch_button 
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    )
target_link_libraries(switch_button 
    pico_stdlib 
    )