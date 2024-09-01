#pragma once

// Platform-Specific Headers for Vulkan
#if defined(TORCH_ENGINE_PLATFORM_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined(TORCH_ENGINE_PLATFORM_MACOS)
#define VK_USE_PLATFORM_MACOS_MVX
#elif defined(TORCH_ENGINE_PLATFORM_LINUX)
#define VK_USE_PLATFORM_XCB_KHR
#else
#error Unsupported platform
#endif

#define TORCH_ENGINE_API_OPENGL

// Standard Library Headers
#include <iostream>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <memory>
#include <stdexcept>
#include <optional>
#include <mutex>
#include <set>
#include <limits>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <array>
#include <sstream>
#include <ppl.h>
#include <shobjidl_core.h>
#include <iomanip>
#include <random>
#include <chrono>
#include <variant>

#define _USE_MATH_DEFINES
#include <math.h>

#define NOMINMAX
#include <Windows.h>

// Third-Party Headers
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async.h>
#include <spdlog/async_logger.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>  
#include <glm/gtc/type_ptr.hpp>  
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

#include <Logger.h>
#include <Ref.h>
#include <FileUtils.h>

// Vulkan Headers
#include <vulkan/vulkan.h>

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

constexpr int MAX_FRAMES_IN_FLIGHT = 2;


