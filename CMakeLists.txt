cmake_minimum_required(VERSION 3.22)

project(Torch)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Get the project root path
set(PROJECT_ROOT_DIR "${CMAKE_SOURCE_DIR}")

# Define a macro PROJECT_ROOT to be used in the code
add_compile_definitions(PROJECT_ROOT="${PROJECT_ROOT_DIR}")

# Assets
add_definitions(-DAD_DEFINE_RES_ROOT_DIR=\"${CMAKE_SOURCE_DIR}/assets/\")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DENABLE_LOGGING)
endif()

if(WIN32)
    message("Platform: Windows")
    add_definitions(-DTORCH_ENGINE_PLATFORM_WIN32)
elseif(UNIX)
    if(APPLE)
        message("Platform: MacOS")
        add_definitions(-DTORCH_ENGINE_PLATFORM_MACOS)
    else()
        message("Platform: Linux")
        add_definitions(-DTORCH_ENGINE_PLATFORM_LINUX)
    endif()
else()
    message("Platform: Unsupported")
endif()

# Find Vulkan package
find_package(Vulkan REQUIRED)

if(Vulkan_FOUND)
    message("----FIND VULKAN----: ", ${Vulkan_INCLUDE_DIRS})
else()
    message("----CANNOT FIND VULKAN----")
endif()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/pch 
    ${CMAKE_SOURCE_DIR}/thirdParty/spdlog
    ${CMAKE_SOURCE_DIR}/thirdParty/glfw/include
    ${CMAKE_SOURCE_DIR}/thirdParty/glad/include
    ${CMAKE_SOURCE_DIR}/thirdParty/imgui
    ${CMAKE_SOURCE_DIR}/thirdParty/glm
    ${CMAKE_SOURCE_DIR}/thirdParty/tinygltf
    ${CMAKE_SOURCE_DIR}/thirdParty/entt/src
    ${CMAKE_SOURCE_DIR}/thirdParty/yaml-cpp/include
    ${CMAKE_SOURCE_DIR}/thirdParty/ImGuizmo
)
# Add subdirectories
add_subdirectory(thirdParty)
add_subdirectory(utils)
add_subdirectory(editor)
add_subdirectory(Core)
add_subdirectory(engine)
add_subdirectory(Sandbox)



