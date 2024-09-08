cmake_minimum_required(VERSION 3.0.0)
project(gptokeyb VERSION 0.1.0 LANGUAGES CXX)

list(APPEND CMAKE_PREFIX_PATH
    "${CMAKE_CURRENT_LIST_DIR}/cmake/modules"
)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Option for enabling X11 and Wayland support
option(ENABLE_X11 "Enable X11 input support" OFF)
# option(ENABLE_WAYLAND "Enable Wayland input support" OFF)

set(SRC_FILES
    src/analog.cpp
    src/config.cpp
    src/input.cpp
    src/output_uinput.cpp
    src/xbox360.cpp
    src/keyboard.cpp
    src/util.cpp
    src/gptokeyb.cpp
    )

set(LINK_LIBS
    ${SDL2_LIBRARIES}
    )

# Conditionally add X11 support
if(ENABLE_X11)
    find_package(X11 REQUIRED)
    find_library(XTEST_LIB Xtst REQUIRED)

    include_directories(${X11_INCLUDE_DIR})

    # Add X11 input source file
    list(APPEND SRC_FILES src/output_x11.cpp)

    # Link with X11 and Xtst libraries
    list(APPEND LINK_LIBS ${X11_LIBRARIES} ${XTEST_LIB})
endif()

# Link with libevdev libraries
find_package(LIBEVDEV REQUIRED)
include_directories(${LIBEVDEV_INCLUDE_DIRS})
list(APPEND LINK_LIBS ${LIBEVDEV_LIBRARIES})

add_executable(
    gptokeyb
    ${SRC_FILES})

# Common target linking
target_link_libraries(
    gptokeyb
    ${LINK_LIBS})


if(ENABLE_X11)
    target_compile_definitions(gptokeyb PRIVATE ENABLE_X11=1)
endif()
