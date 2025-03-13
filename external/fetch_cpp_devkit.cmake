#===========================  NOTICE  =====================================
# this file must be copied into the top level CMakeList.txt file of a project that would like to use CPP_DevKit library.
# it must be inserted after the cmake project() command

#====  defining CPP_DevKit src and include  ===============================
set(CPP_DEVKIT_PATH "C:/Users/chris/OneDrive/MyPico_DevKit/CPP_DevKit/src")
include_directories(${CPP_DevKit_SOURCE_DIR}/include) # used to define include directories 

#====  importing source  ==================================================
include(FetchContent)
FetchContent_Declare(CPP_DevKit SOURCE_DIR ${CPP_DEVKIT_PATH})
FetchContent_MakeAvailable(CPP_DevKit)

#====  can be removed  ====================================================
message("----->CPP_DevKit_SOURCE_DIR/src : ${CPP_DevKit_SOURCE_DIR}/src")   
message("----->PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")                 
