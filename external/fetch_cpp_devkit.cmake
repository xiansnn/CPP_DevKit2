#===========================  NOTICE  =====================================
# this file must be copied into the top level CMakeList.txt file of a project that would like to use CPP_DevKit library.
# it must be inserted after the sequence
# 
#  include(pico_sdk_import.cmake)
#  project(<calling_project>)
#  pico_sdk_init()

#====  defining CPP_DevKit src and include  ===============================
set (ENV{CPP_DEVKIT_PATH} "C:/Users/chris/OneDrive/MyPico_DevKit/CPP_DevKit")
set (ENV{CPP_DEVKIT_INC_PATH} "C:/Users/chris/OneDrive/MyPico_DevKit/CPP_DevKit/src")
message("-fetch_cpp_devkit-10--->ENV{CPP_DEVKIT_PATH} : $ENV{CPP_DEVKIT_PATH}") 
message("-fetch_cpp_devkit-11--->ENV{CPP_DEVKIT_INC_PATH} : $ENV{CPP_DEVKIT_INC_PATH}") 

#====  importing source  ==================================================
include(FetchContent)
FetchContent_Declare(CPP_DevKit SOURCE_DIR $ENV{CPP_DEVKIT_PATH})
FetchContent_MakeAvailable(CPP_DevKit)

#====  setting include directories 
#      for the calling project   =====================
include_directories($ENV{CPP_DEVKIT_INC_PATH}) # used to define include directories 