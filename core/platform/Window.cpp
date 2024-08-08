#include "Window.h"
#include "TorchWindow.h"

namespace core
{
    std::unique_ptr<Window> Window::Create(const WindowSpecification& specification)
    {
#ifdef TORCH_ENGINE_PLATFORM_WIN32
        // Perform platform-specific initialization for Windows if needed
        return std::make_unique<TorchWindow>(specification);
#elif defined(TORCH_ENGINE_PLATFORM_MACOS)
        // Perform platform-specific initialization for macOS if needed
        return std::make_unique<TorchWindow>(specification);
#elif defined(TORCH_ENGINE_PLATFORM_LINUX)
        // Perform platform-specific initialization for Linux if needed
        return std::make_unique<TorchWindow>(specification);
#else
        // Handle unsupported platform or provide a default behavior
        TORCH_LOG_ERROR("Unsupported Platform");
        return nullptr;
#endif
    }

    Window::~Window() = default;  // Correctly defined pure virtual destructor
}
