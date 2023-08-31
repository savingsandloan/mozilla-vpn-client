# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

add_custom_target(build_sentry_tests)
set_target_properties(build_sentry_tests PROPERTIES
    EXCLUDE_FROM_ALL TRUE
    FOLDER "Tests"
)

include(FetchContent)
FetchContent_Declare(
googletest
URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)
set(CMAKE_CXX_STANDARD 20)

# Creates a new Executable target
# Auto Links to QT::Core and 
# GTest, also set's up test case registration
function(add_qt_gtest TARGET)
    add_executable(${TARGET})    
    add_dependencies(build_sentry_tests ${TARGET})
    target_link_libraries(${TARGET} 
        Qt6::Core
        GTest::gtest_main
    )   
    gtest_discover_tests(${TARGET})
endfunction(add_qt_gtest )

add_qt_gtest(watchdog_test)
target_sources(watchdog_test PRIVATE
  test_watchdog.cc
  ../../sentrywatchdog.cpp
  ../../sentrywatchdog.h
)








