#include "ServiceLocator.h"

namespace utils
{
    std::optional<std::unique_ptr<core::Window>> ServiceLocator::s_Window = std::nullopt;
    std::mutex ServiceLocator::s_WindowMutex;

    void ServiceLocator::RegisterWindow(std::unique_ptr<core::Window> window)
    {
        std::lock_guard<std::mutex> lock(s_WindowMutex);
        TORCH_LOG_INFO("Window registered");

        s_Window = std::move(window);
    }

    core::Window* ServiceLocator::GetWindow()
    {
        std::lock_guard<std::mutex> lock(s_WindowMutex);

        if (!s_Window.has_value())
        {
            TORCH_LOG_CRITICAL("Window not registered");
            throw std::runtime_error("Window not registered");
        }
        return s_Window->get();
    }
}
