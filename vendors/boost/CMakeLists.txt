file(
    GLOB
    BOOST_SOURCES
    *
)

set(
    IGNORED_FOLDERS
)

list(
    REMOVE_ITEM
    BOOST_SOURCES
    ${IGNORED_FOLDERS}
)

foreach(BOOST_SOURCE ${BOOST_SOURCES})
    if(IS_DIRECTORY ${BOOST_SOURCE})
        add_subdirectory(${BOOST_SOURCE})
    endif()
endforeach()