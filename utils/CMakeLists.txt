# Define Utils as a library with source files
add_library(Utils STATIC Logger.cpp ServiceLocator.cpp "FileUtils.h" "FileUtils.cpp" "Ref.h" "UUID.h" "UUID.cpp" "Serializer.h" "Serializer.cpp" "TemplateUtils.h")

# Specify the precompiled header
target_precompile_headers(Utils PRIVATE ${CMAKE_SOURCE_DIR}/pch/pch.h)

# Include directories for Utils
target_include_directories(Utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR})

# Link spdlog directly to Utils and Vulkan
target_link_libraries(Utils PUBLIC spdlog::spdlog glad glfw Vulkan::Vulkan Core yaml-cpp::yaml-cpp)
target_include_directories(Utils PRIVATE ${Vulkan_INCLUDE_DIRS})


