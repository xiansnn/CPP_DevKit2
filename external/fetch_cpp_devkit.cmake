#===========================  NOTICE  =====================================
# this file must be copied into the top level CMakeList.txt file of a project that would like to use CPP_DevKit library.
# it must be inserted after the cmake project() command

#====  defining CPP_DevKit src and include  ===============================
set(CPP_DEVKIT_PATH "C:/Users/chris/OneDrive/MyPico_DevKit/CPP_DevKit")
include_directories(${CPP_DEVKIT_PATH}/include) # used to define include directories 

#====  importing source  ==================================================
include(FetchContent)
FetchContent_Declare(CPP_DevKit SOURCE_DIR ${CPP_DEVKIT_PATH}/src)
FetchContent_MakeAvailable(CPP_DevKit)

#====  can be removed  ====================================================
message("----->CPP_DevKit_SOURCE_DIR/src : ${CPP_DEVKIT_PATH}/src")   
message("----->CPP_DevKit_SOURCE_DIR/include : ${CPP_DEVKIT_PATH}/include")   
message("----->PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")  