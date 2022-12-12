cmake_minimum_required(VERSION 3.11.0)

set(PROJECT_NAME uron)

project(
  ${PROJECT_NAME}
  VERSION 0.0.1
  LANGUAGES CXX
)

find_package(Vulkan REQUIRED)
message(STATUS "Found Vulkan: $ENV{VULKAN_SDK}")

find_package(glfw3 3.3 REQUIRED)
set(GLFW_LIB glfw)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build")
include_directories(external)
target_include_directories(
  ${PROJECT_NAME} PUBLIC
  ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(${PROJECT_NAME} glfw ${Vulkan_LIBRARIES})

# Find all vertex and fragment sources within shaders directory
# taken from VBlancos vulkan tutorial
# https://github.com/vblanco20-1/vulkan-guide/blob/all-chapters/CMakeLists.txt
find_program(GLSL_VALIDATOR glslangValidator HINTS
  /usr/bin
  /usr/local/bin
)

file(GLOB_RECURSE GLSL_SOURCE_FILES
  "${PROJECT_SOURCE_DIR}/shaders/*.frag"
  "${PROJECT_SOURCE_DIR}/shaders/*.vert"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
  Shaders
  DEPENDS ${SPIRV_BINARY_FILES}
)
add_dependencies(${PROJECT_NAME} Shaders)
