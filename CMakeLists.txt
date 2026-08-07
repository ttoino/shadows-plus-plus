cmake_minimum_required(VERSION 3.27)

project(borders-plus-plus
    DESCRIPTION "shadows-plus-plus plugin for Hyprland"
    VERSION 0.1
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(deps REQUIRED IMPORTED_TARGET
    hyprland
    libdrm
    libinput
    libudev
    pangocairo
    pixman-1
    wayland-server
    xkbcommon
)

file(GLOB_RECURSE SRC "*.cpp")
list(FILTER SRC EXCLUDE REGEX "^${CMAKE_CURRENT_SOURCE_DIR}/tests/")
list(FILTER SRC EXCLUDE REGEX "^${CMAKE_CURRENT_SOURCE_DIR}/.*/tests/")

add_library(shadows-plus-plus SHARED ${SRC})
target_include_directories(shadows-plus-plus PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(shadows-plus-plus PRIVATE rt PkgConfig::deps)

install(TARGETS shadows-plus-plus)

include(CTest)

if(BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)
    enable_testing()

    pkg_check_modules(testdeps REQUIRED IMPORTED_TARGET hyprutils)

    file(GLOB_RECURSE TESTFILES "tests/*.cpp")
    add_executable(shadows-plus-plus-tests ${TESTFILES} geometry.cpp)
    target_include_directories(shadows-plus-plus-tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(shadows-plus-plus-tests PRIVATE PkgConfig::testdeps GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(shadows-plus-plus-tests)
endif()
