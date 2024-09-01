# Define the Engine library target
add_library(Editor STATIC)

# Specify the source files for the Editor library
file(GLOB_RECURSE ENGINE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/**/*.h
)

# Add the source files to the Editor library
target_sources(Editor PRIVATE ${ENGINE_SOURCES})

# Specify the precompiled header
target_precompile_headers(Editor PRIVATE ${CMAKE_SOURCE_DIR}/pch/pch.h)

# Include directories for the Editor library
target_include_directories(Editor PRIVATE ${CMAKE_SOURCE_DIR} ${Vulkan_INCLUDE_DIRS})

# Link the Utils library and Vulkan to the Editor library
target_link_libraries(Editor PRIVATE Utils glfw glad Vulkan::Vulkan imgui Core yaml-cpp::yaml-cpp EnTT::EnTT ImGuizmo)
