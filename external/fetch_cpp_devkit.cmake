#===========================  NOTICE  =====================================
# this file must be copied into the top level CMakeList.txt file of a project that would like to use CPP_DevKit library.
# it must be inserted after the cmake project() command

#====  defining CPP_DevKit src and include  ===============================
set(CPP_DEVKIT_SRC_PATH "C:/Users/chris/OneDrive/MyPico_DevKit/CPP_DevKit/src")
set(CPP_DEVKIT_INC_PATH "C:/Users/chris/OneDrive/MyPico_DevKit/CPP_DevKit/include")
message("-freq->CPP_DEVKIT_SRC_PATH : ${CPP_DEVKIT_SRC_PATH}")  

#====  setting include directory  ==================================================
message("-freq->CPP_DEVKIT_INC_PATH : ${CPP_DEVKIT_INC_PATH}")   
include_directories(${CPP_DEVKIT_INC_PATH}) # used to define include directories 


#====  importing source  ==================================================
include(FetchContent)
FetchContent_Declare(CPP_DevKit SOURCE_DIR ${CPP_DEVKIT_SRC_PATH})
FetchContent_MakeAvailable(CPP_DevKit)
