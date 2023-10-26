# Defines which OS builds can include loophole. Check src/cmake Lists for all values of MZ_PLATFORM_NAME
set(LOOPHOLE_OS  "Windows") #TODO:"Darwin" "Linux"

LIST(FIND LOOPHOLE_OS ${CMAKE_SYSTEM_NAME} LOOPHOLE_SUPPORTED)

if( ${LOOPHOLE_SUPPORTED} EQUAL -1 )
    return()
endif()

add_custom_target(loophole ALL
    BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/loophole.exe
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} -E env 
                GOCACHE=${CMAKE_BINARY_DIR}/go-cache
            go build -o "${CMAKE_CURRENT_BINARY_DIR}/loophole.exe"
)
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_BINARY_DIR}/go-cache)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/loophole.exe
    DESTINATION .
)
