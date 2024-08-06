# Define Logger as a library with source files
add_library(Logger STATIC Logger.cpp)

# Specify the precompiled header
target_precompile_headers(Logger PRIVATE ${CMAKE_SOURCE_DIR}/pch/pch.h)

# Include directories for Logger
target_include_directories(Logger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link spdlog directly to Logger and Vulkan
target_link_libraries(Logger PUBLIC spdlog::spdlog glad glfw Vulkan::Vulkan)
target_include_directories(Logger PRIVATE ${Vulkan_INCLUDE_DIRS})
