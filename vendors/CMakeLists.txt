file(
    GLOB 
    VENDORS_SOURCES
    *
)

set(
    EXCLUDES_VENDORS
)

list(
    REMOVE_ITEM
    VENDORS_SOURCES
    ${EXCLUDES_VENDORS}
)

foreach(VENDOR_SOURCE ${VENDORS_SOURCES})
    if(IS_DIRECTORY ${VENDOR_SOURCE})
        add_subdirectory(${VENDOR_SOURCE})
    endif()
endforeach()
