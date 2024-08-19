# Define the Engine library target
add_library(Engine STATIC)

# Specify the source files for the Engine library
file(GLOB_RECURSE ENGINE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/**/*.h
)

# Add the source files to the Engine library
target_sources(Engine PRIVATE ${ENGINE_SOURCES})

# Specify the precompiled header
target_precompile_headers(Engine PRIVATE ${CMAKE_SOURCE_DIR}/pch/pch.h)

# Include directories for the Engine library
target_include_directories(Engine PRIVATE ${CMAKE_SOURCE_DIR} ${Vulkan_INCLUDE_DIRS})

# Link the Utils library and Vulkan to the Engine library
target_link_libraries(Engine PRIVATE Utils glfw glad Vulkan::Vulkan Core Editor)
