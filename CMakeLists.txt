# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")
SET(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/FreeRTOS-Kernel)
# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(CPP_DevKit C CXX ASM)


# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

#-------------------------------------------------------------------------------------
if(NOT PROJECT_IS_TOP_LEVEL)
# the fetch_cpp_devkit.cmake list should have set the ENV{CPP_DEVKIT_INC_PATH}
message("-cpp_devkit-ENV{CPP_DEVKIT_INC_PATH}-> : $ENV{CPP_DEVKIT_INC_PATH}" )
include_directories($ENV{CPP_DEVKIT_INC_PATH})
add_subdirectory(src)
endif()
#-------------------------------------------------------------------------------------
if(PROJECT_IS_TOP_LEVEL)
    # Find Doxygen 
    find_package(Doxygen) 
    if(DOXYGEN_FOUND) # Set input and output files 
        set(DOXYGEN_CNF ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile_cnf) 
        add_custom_target(doc_doxygen 
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CNF}        
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen" VERBATIM) 
    endif()

    set(CPP_DEVKIT_INC_PATH ${CMAKE_CURRENT_LIST_DIR}/src)
    include_directories(
        ${CPP_DEVKIT_INC_PATH} 
        ${CMAKE_CURRENT_LIST_DIR})
    include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)
    add_subdirectory(src)
    add_subdirectory(tests)
    add_subdirectory(test_freeRTOS)
endif()
#-------------------------------------------------------------------------------------