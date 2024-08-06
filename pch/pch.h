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

// Third-Party Headers
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async.h>
#include <spdlog/async_logger.h>

#include <Logger.h>

// Vulkan Headers
#include <vulkan/vulkan.h> 

#include <glad/gl.h>
#include <GLFW/glfw3.h>
