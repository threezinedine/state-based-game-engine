file(
    GLOB 
    SCRIPT_FILES
    *.cpp
)

foreach(SCRIPT_FILE ${SCRIPT_FILES})
    message(STATUS "Handling script file: ${SCRIPT_FILE}")
endforeach()
