# Defines which OS builds can include loophole. Check src/cmake Lists for all values of MZ_PLATFORM_NAME
if(LINUX)
    return()
endif()

if(WIN32)
    set(LOOPHOLE_BIN  "loophole.exe")
else()
    set(LOOPHOLE_BIN  "loophole")
endif()


add_custom_target(loophole ALL
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} -E env 
                GOCACHE=${CMAKE_BINARY_DIR}/go-cache
            go build -o "${CMAKE_CURRENT_BINARY_DIR}/${LOOPHOLE_BIN}"
)
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_BINARY_DIR}/go-cache)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${LOOPHOLE_BIN}
    DESTINATION .
)
