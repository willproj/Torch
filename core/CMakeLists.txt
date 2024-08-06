# Define the Engine library target
add_library(Core STATIC)

# Specify the source files for the Engine library
file(GLOB_RECURSE ENGINE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/**/*.h
)

# Add the source files to the Engine library
target_sources(Core PRIVATE ${ENGINE_SOURCES})

# Specify the precompiled header
target_precompile_headers(Core PRIVATE ${CMAKE_SOURCE_DIR}/pch/pch.h)

# Include directories for the Engine library
target_include_directories(Core PRIVATE ${CMAKE_SOURCE_DIR} ${Vulkan_INCLUDE_DIRS})

# Link the Logger library and Vulkan to the Engine library
target_link_libraries(Core PRIVATE Logger glfw glad Vulkan::Vulkan)
