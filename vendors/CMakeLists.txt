file(
    GLOB 
    VENDORS_SOURCES
    *
)

foreach(VENDOR_SOURCE ${VENDORS_SOURCES})
    if (IS_DIRECTORY ${VENDOR_SOURCE})
        if(VENDOR_SOURCE STREQUAL "raylib")
            set(BUILD_SHARED_LIBS FALSE) 
        endif()
        
        add_subdirectory(${VENDOR_SOURCE})
        message(STATUS "Added Vendor: ${VENDOR_SOURCE}")
    endif()
endforeach()
