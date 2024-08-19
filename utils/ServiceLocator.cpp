#include "ServiceLocator.h"

namespace utils
{
    std::optional<std::unique_ptr<core::Window>> ServiceLocator::s_Window = std::nullopt;
    std::optional<std::unique_ptr<core::Keyboard>> ServiceLocator::s_Keyboard = std::nullopt;
    std::optional<std::unique_ptr<core::Mouse>> ServiceLocator::s_Mouse = std::nullopt;
    std::optional<std::unique_ptr<core::TorchGraphicsContext>> ServiceLocator::s_GraphicsContext = std::nullopt;

    std::mutex ServiceLocator::s_WindowMutex;
    std::mutex ServiceLocator::s_KeyboardMutex;
    std::mutex ServiceLocator::s_MouseMutex;
    std::mutex ServiceLocator::s_GraphicsContextMutex;

    void ServiceLocator::RegisterWindow(std::unique_ptr<core::Window> window)
    {
        std::lock_guard<std::mutex> lock(s_WindowMutex);
        TORCH_LOG_INFO("Window registered");

        s_Window = std::move(window);
    }

    void ServiceLocator::RegisterKeyboard(std::unique_ptr<core::Keyboard> keyboard)
    {
        std::lock_guard<std::mutex> lock(s_KeyboardMutex);
        TORCH_LOG_INFO("Keyboard registered");

        s_Keyboard = std::move(keyboard);
    }

    void ServiceLocator::RegisterMouse(std::unique_ptr<core::Mouse> mouse)
    {
        std::lock_guard<std::mutex> lock(s_MouseMutex);
        TORCH_LOG_INFO("Mouse registered");

        s_Mouse = std::move(mouse);
    }

    void ServiceLocator::RegisterGraphicsContext(std::unique_ptr<core::TorchGraphicsContext> graphicsContext)
    {
        std::lock_guard<std::mutex> lock(s_GraphicsContextMutex);
        TORCH_LOG_INFO("graphics context registered");

        s_GraphicsContext = std::move(graphicsContext);

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

    core::Keyboard* ServiceLocator::GetKeyboard()
    {
        std::lock_guard<std::mutex> lock(s_KeyboardMutex);

        if (!s_Keyboard.has_value())
        {
            TORCH_LOG_CRITICAL("Keyboard not registered");
            throw std::runtime_error("Keyboard not registered");
        }
        return s_Keyboard->get();
    }

    core::Mouse* ServiceLocator::GetMouse()
    {
        std::lock_guard<std::mutex> lock(s_MouseMutex);

        if (!s_Mouse.has_value())
        {
            TORCH_LOG_CRITICAL("Mouse not registered");
            throw std::runtime_error("Mouse not registered");
        }
        return s_Mouse->get();
    }

    core::TorchGraphicsContext* ServiceLocator::GetGraphicsContext()
    {
        std::lock_guard<std::mutex> lock(s_GraphicsContextMutex);

        if (!s_GraphicsContext.has_value())
        {
            TORCH_LOG_CRITICAL("Mouse not registered");
            throw std::runtime_error("Mouse not registered");
        }
        return s_GraphicsContext->get();
    }
    
}
