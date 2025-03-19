

file(GLOB ALL_ITEMS LIST_DIRECTORIES true  ${CMAKE_CURRENT_LIST_DIR}/*  )
foreach(ITEM ${ALL_ITEMS})
    if(IS_DIRECTORY ${ITEM})
        get_filename_component(SHORT_NAME ${ITEM} NAME)
        message( STATUS "---cpp_devkit add utilities library-> ${SHORT_NAME}")
        add_subdirectory(${SHORT_NAME})
    endif()
endforeach(ITEM)
