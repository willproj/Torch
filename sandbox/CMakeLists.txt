# Define the executable target
add_executable(Sandbox main.cpp)

# Specify the precompiled header
target_precompile_headers(Sandbox PRIVATE ${CMAKE_SOURCE_DIR}/pch/pch.h)

# Include directories for the platform target
target_include_directories(Sandbox PRIVATE ${CMAKE_SOURCE_DIR})

# Link Engine, Logger, and Vulkan libraries to the Sandbox executable
target_link_libraries(Sandbox PRIVATE Logger Engine glad glfw Vulkan::Vulkan)
