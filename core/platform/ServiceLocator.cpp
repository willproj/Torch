#include "ServiceLocator.h"

namespace core
{
    std::unique_ptr<Window> ServiceLocator::s_Window = nullptr;
    void ServiceLocator::RegisterWindow(std::unique_ptr<Window> window)
    {
        TORCH_LOG_INFO("Window registered");
        s_Window = std::move(window);
    }

    Window* ServiceLocator::GetWindow()
    {
        if (!s_Window)
        {
            TORCH_LOG_CRITICAL("Window not registered");
            throw std::runtime_error("Window not registered");
        }
        return s_Window.get();
    }
}