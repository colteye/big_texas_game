# Project definition.
cmake_minimum_required(VERSION 3.1)
project(big_texas_game)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/bin)

# Source and Header files.
get_filename_component(PARENT_DIR ../ ABSOLUTE)

set(SRC_DIR "${PARENT_DIR}/src")
set(OUT_SPRITE_DIR "${PARENT_DIR}/src/sprite_cbt")
set(LIB_DIR "${PARENT_DIR}/lib")
set(SOURCES "${SRC_DIR}/main.c"
"${SRC_DIR}/input.c"
"${SRC_DIR}/object.c"
"${SRC_DIR}/physics.c"
"${SRC_DIR}/player.c"
"${SRC_DIR}/render.c"
"${SRC_DIR}/scene.c"
"${SRC_DIR}/sprite.c"
"${SRC_DIR}/main.c"
"${SRC_DIR}/driver/gl_driver.c")

set(HEADERS "${SRC_DIR}/bounds.h"
"${SRC_DIR}/define.h"
"${SRC_DIR}/input.h"
"${SRC_DIR}/object.h"
"${SRC_DIR}/physics.h"
"${SRC_DIR}/player.h"
"${SRC_DIR}/render.h"
"${SRC_DIR}/scene.h"
"${SRC_DIR}/sprite.h"
"${SRC_DIR}/vec.h"
"${SRC_DIR}/driver/gl_driver.h")

# Copy sprites into executable dir.
file(GLOB SPRITES "${OUT_SPRITE_DIR}/*.cbt")	
file(COPY ${SPRITES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)

file(GLOB ALPHA_SPRITES "${OUT_SPRITE_DIR}/*.cbta")	 
file(COPY ${ALPHA_SPRITES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)

# Executable definition and properties.
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}")
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 99)

# GLFW.
set(GLFW_DIR "${LIB_DIR}/glfw")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory("${GLFW_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/glfw_build")
target_link_libraries(${PROJECT_NAME} "glfw" "${GLFW_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")

# glad.
set(GLAD_DIR "${LIB_DIR}/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")

