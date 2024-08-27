# NOTE:  If you installed SDL2 at an unusual place and FindSDL2 fails,
#        you can define SDL2_INCLUDE_DIR on the cmdline.  For example:
#        cmake -DSDL2_INCLUDE_DIR=/opt/SDL2/include/SDL2 [other stuff]

cmake_minimum_required(VERSION 3.0.0)
project(gptokeyb
        VERSION 0.1.0
        LANGUAGES CXX)

list(APPEND CMAKE_PREFIX_PATH
    "${CMAKE_CURRENT_LIST_DIR}/cmake/modules"
)

# Define an option to choose between libevdev and X11
option(USE_X11 "Use X11 for input handling instead of libevdev" OFF)

# Common dependencies
find_package(SDL2 REQUIRED)

if(USE_X11)
    # Find X11 libraries if the option is enabled
    find_package(X11 REQUIRED)
    find_library(XTEST_LIB Xtst REQUIRED)
    include_directories(${X11_INCLUDE_DIR})

    # Add a preprocessor define for USE_X11
    add_definitions(-DUSE_X11)
else()
    # Find libevdev libraries if the option is disabled
    find_package(LIBEVDEV REQUIRED)
    include_directories(${LIBEVDEV_INCLUDE_DIRS})
endif()

include_directories(${SDL2_INCLUDE_DIRS})

set(EXTRA_CXXFLAGS)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  set(EXTRA_CXXFLAGS "${EXTRA_CXXFLAGS} -Wall")
endif()

add_executable(gptokeyb
    src/analog.cpp
    src/config.cpp
    src/input.cpp
    src/xbox360.cpp
    src/keyboard.cpp
    src/util.cpp
    src/gptokeyb.cpp
)

if(USE_X11)
    # Link with X11 and Xtst libraries
    target_link_libraries(gptokeyb ${SDL2_LIBRARIES} ${X11_LIBRARIES} ${XTEST_LIB})
else()
    # Link with libevdev libraries
    target_link_libraries(gptokeyb ${SDL2_LIBRARIES} ${LIBEVDEV_LIBRARIES})
endif()
