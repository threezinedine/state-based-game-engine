file(
    GLOB 
    VENDORS_SOURCES
    *
)

set(BUILD_SHARED_LIBS FALSE) 

foreach(VENDOR_SOURCE ${VENDORS_SOURCES})
    if (IS_DIRECTORY ${VENDOR_SOURCE})
        add_subdirectory(${VENDOR_SOURCE})
        message(STATUS "Added Vendor: ${VENDOR_SOURCE}")
    endif()
endforeach()
