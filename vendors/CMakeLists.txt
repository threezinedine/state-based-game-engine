file(
    GLOB
    VENDORS_SOURCES
    *
)

set(
    IGNORED_FOLDERS
)

list(
    REMOVE_ITEM
    VENDORS_SOURCES
    ${IGNORED_FOLDERS}
)

foreach(VENDORS_SOURCE ${VENDORS_SOURCES})
    if(IS_DIRECTORY ${VENDORS_SOURCE})
        add_subdirectory(${VENDORS_SOURCE})
    endif()
endforeach()
