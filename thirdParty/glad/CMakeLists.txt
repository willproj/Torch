cmake_minimum_required(VERSION 3.22)
project(glad)

# Set the GLAD library
add_library(glad STATIC
   ${CMAKE_CURRENT_SOURCE_DIR}/src/gl.c
   ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkan.c
)

# Specify include directories
target_include_directories(glad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Linking Vulkan in the glad target
target_link_libraries(glad PRIVATE Vulkan::Vulkan)