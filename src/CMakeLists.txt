include_directories(${CPP_DevKit_SOURCE_DIR}/include)

message( STATUS "----> src include dir : ${CPP_DevKit_SOURCE_DIR}/include")

file(GLOB ALL_ITEMS LIST_DIRECTORIES true  ${CMAKE_CURRENT_LIST_DIR}/*  )
foreach(ITEM ${ALL_ITEMS})
    if(IS_DIRECTORY ${ITEM})
        get_filename_component(SHORT_NAME ${ITEM} NAME)
        message( STATUS "----> src library dir : ${SHORT_NAME}")
        add_subdirectory(${SHORT_NAME})
    endif()
endforeach(ITEM)
