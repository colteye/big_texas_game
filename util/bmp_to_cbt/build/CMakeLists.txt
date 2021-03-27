# Project definition.
cmake_minimum_required(VERSION 3.1)
project(bmp_to_cbt)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/bin)

# Source and Header files.
get_filename_component(PARENT_DIR ../ ABSOLUTE)

set(SRC_DIR "${PARENT_DIR}/src")
set(OUT_SPRITE_DIR "${PARENT_DIR}/src/sprite_bmp")
set(LIB_DIR "${PARENT_DIR}/lib")
set(SOURCES "${SRC_DIR}/bmp_to_cbt.c")

# Copy sprites into executable dir.
file(GLOB SPRITES "${OUT_SPRITE_DIR}/*.bmp")	
file(COPY ${SPRITES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)

# Executable definition and properties.
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}")
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 99)
