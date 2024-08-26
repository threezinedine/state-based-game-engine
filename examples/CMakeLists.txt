file(
    GLOB 
    EXAMPLES
    *
)


foreach(EXAMPLE ${EXAMPLES})
    if(IS_DIRECTORY ${EXAMPLE})
        add_subdirectory(${EXAMPLE})
    endif()
endforeach()
