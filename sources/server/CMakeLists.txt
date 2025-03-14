cmake_minimum_required(VERSION 3.20)
project(server)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Include Conan-generated files
include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake)

# Include directories for dependencies
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")

# Add hpp's paths here
set(INCLUDE_DIRS include include/engine include/engine/component include/game include/game/system include/game/scene include/network)
include_directories(${INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

set(THEME
    "DEFAULT"
    CACHE STRING "Select the theme to use")
set_property(CACHE THEME PROPERTY STRINGS "DEFAULT" "DARK" "LIGHT")
if(THEME STREQUAL "DEFAULT")
    add_definitions(-DTHEME=0)
elseif(THEME STREQUAL "DARK")
    add_definitions(-DTHEME=1)
elseif(THEME STREQUAL "LIGHT")
    add_definitions(-DTHEME=2)
endif()

# Source files (using wildcards)
file(GLOB_RECURSE SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/game/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/game/scene/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/game/system/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/network/*.cpp"
)

add_executable(server ${SOURCE_FILES})

# Include Conan dependencies
find_package(asio REQUIRED)
target_link_libraries(server PRIVATE asio::asio)

# Set the output directory to the root
set_target_properties(server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../
)

# **Set the output directory to the root only if not building inside Docker**
option(BUILD_IN_DOCKER "Build in Docker container" OFF)

if(NOT BUILD_IN_DOCKER)
    set_target_properties(server PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../
    )
endif()

# Doxygen setup for unified documentation
option(BUILD_DOC "Build documentation" ON)

find_package(Doxygen REQUIRED)

if(DOXYGEN_FOUND)
    # Path to the root-level Doxyfile.in
    set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/doxygen/Doxyfile.in)
    set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    set(DOXYGEN_INPUT_DIR "${CMAKE_SOURCE_DIR}")
    set(DOXYGEN_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/docs/doxygen")

    configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

    add_custom_target(doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating unified API documentation with Doxygen for server"
        VERBATIM
    )
else()
    message(FATAL_ERROR "Server - Doxygen is required to build the documentation.")
endif()
