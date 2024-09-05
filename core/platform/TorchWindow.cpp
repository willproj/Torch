#include "TorchWindow.h"
#include "core/events/WindowEvent.h"
#include "core/events/MouseEvents.h"
#include "core/events/KeyboardEvents.h"
#include "core/events/Keyboard.h"
#include <utils/ServiceLocator.h>

namespace core
{
    TorchWindow::TorchWindow(const WindowSpecification &specification)
        : Window(specification)
    {
        if (!glfwInit())
        {
            TORCH_LOG_ERROR("Failed to initialzie GLFW.");
            return;
        }
#ifdef TORCH_ENGINE_API_OPENGL
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#else
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif
        m_Specification.glfwWindow = glfwCreateWindow(m_Specification.width, m_Specification.height, m_Specification.title.c_str(), nullptr, nullptr);
        if (!m_Specification.glfwWindow)
        {
            TORCH_LOG_ERROR("Failed to create GLFW window.");
            return;
        }

        glfwMakeContextCurrent(m_Specification.glfwWindow);

#ifdef TORCH_ENGINE_API_OPENGL
        if (!gladLoadGL(glfwGetProcAddress))
        {
            TORCH_LOG_ERROR("Failed to initialize GLAD.");
            return;
        }
#endif

        glfwSetWindowUserPointer(m_Specification.glfwWindow, this);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        // set window size call back function
        glfwSetWindowSizeCallback(m_Specification.glfwWindow, [](GLFWwindow *window, int width, int height)
		{
            auto* torchWindow = static_cast<TorchWindow*>(glfwGetWindowUserPointer(window));
            torchWindow->SetWindowSize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
            glViewport(0, 0, width, height);
            WindowResizeEvent event(width, height);
			torchWindow->OnEvent(event); 
        });

        // set window close callback
        glfwSetWindowCloseCallback(m_Specification.glfwWindow, [](GLFWwindow *window)
		{
            auto* torchWindow = static_cast<TorchWindow*>(glfwGetWindowUserPointer(window));
            TORCH_LOG_DEBUG("[{}:{}] window closed.", __FILE__, __LINE__);

            bool flag = true;
            if (!flag)
            {
                glfwSetWindowShouldClose(window, GLFW_FALSE);
            } 
         });

        // get mouse position
        glfwSetCursorPosCallback(m_Specification.glfwWindow, [](GLFWwindow *window, double xpos, double ypos)
        {
            auto* torchWindow = static_cast<TorchWindow*>(glfwGetWindowUserPointer(window));

            MouseMoveEvent event(xpos, ypos);
            torchWindow->OnEvent(event);
        });

        glfwSetMouseButtonCallback(m_Specification.glfwWindow, [](GLFWwindow *window, int button, int action, int mode)
        {
            auto* torchWindow = static_cast<TorchWindow*>(glfwGetWindowUserPointer(window));

            MouseButtonType mouseButton;
            switch (button)
            {
            case GLFW_MOUSE_BUTTON_LEFT:
                mouseButton = MouseButtonType::LEFT;
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                mouseButton = MouseButtonType::RIGHT;
                break;
            default:
                return; // Unknown button, do nothing
            }

            if (action == GLFW_PRESS)
            {
                MousePressEvent event(static_cast<uint32_t>(mouseButton));
                torchWindow->OnEvent(event);
            }
            else if (action == GLFW_RELEASE)
            {
                MouseReleaseEvent event(static_cast<uint32_t>(mouseButton));
                torchWindow->OnEvent(event);
            } 
        });

        glfwSetScrollCallback(m_Specification.glfwWindow, [](GLFWwindow *window, double xoffset, double yoffset)
        {
            auto* torchWindow = static_cast<TorchWindow*>(glfwGetWindowUserPointer(window));
            MouseScrollEvent event(xoffset, yoffset);
            torchWindow->OnEvent(event); 
        });

        glfwSetKeyCallback(m_Specification.glfwWindow, [](GLFWwindow *window, int key, int scancode, int action, int mods)
        {
                auto* torchWindow = static_cast<TorchWindow*>(glfwGetWindowUserPointer(window));
                if (action == GLFW_PRESS)
                {
                    // Handle key press event
                    KeyPressEvent event(key, scancode, mods);
                    torchWindow->OnEvent(event);
                }
                else if (action == GLFW_RELEASE)
                {
                    // Handle key release event
                    KeyReleaseEvent event(key, scancode, mods);
                    torchWindow->OnEvent(event);
                }
                else if (action == GLFW_REPEAT)
                {
                    // Handle key repeat event
                    KeyRepeatEvent event(key, scancode, mods);
                    torchWindow->OnEvent(event);
                } 
        });

        RegisteEventHandlers();
    }

    bool TorchWindow::ShouldClose() const noexcept
    {
        return glfwWindowShouldClose(m_Specification.glfwWindow);
    }

    void TorchWindow::PollEvents() noexcept
    {
        glfwPollEvents();
    }

    void TorchWindow::SwapBuffers() noexcept
    {
        glfwSwapBuffers(m_Specification.glfwWindow);
    }

    void TorchWindow::SetWindowSize(uint32_t width, uint32_t height) noexcept
    {
        m_Specification.width = width;
        m_Specification.height = height;
        m_IsResized = true;
    }

    void TorchWindow::OnEvent(Event &event)
    {
        auto it = m_EventHandlers.find(event.GetType());
        if (it != m_EventHandlers.end())
        {
            it->second(event);
            if (event.GetType() == EventType::WINDOW_RESIZE)
            {
                m_IsResized = true;
            }
        }
        else
        {
            TORCH_LOG_DEBUG("No handler found for event type: {}", static_cast<uint32_t>(event.GetType()));
        }
    }

    void TorchWindow::ResetIsResize() noexcept
    {
        m_IsResized = false;
    }

    void TorchWindow::HandleMinimization() noexcept
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(m_Specification.glfwWindow, &width, &height);
        while (width == 0 || height == 0)
        {
            glfwGetFramebufferSize(m_Specification.glfwWindow, &width, &height);
            glfwWaitEvents();
        }
    }

    void TorchWindow::RegisteEventHandlers()
    {
        m_EventHandlers[EventType::MOUSE_PRESS] = [this](Event &e)
        {
            auto &event = static_cast<MousePressEvent &>(e);
            utils::ServiceLocator::GetMouse()->OnEvent(event);
            //TORCH_LOG_DEBUG("Mouse Pressed: button {}", event.GetButton());
        };

        m_EventHandlers[EventType::MOUSE_RELEASE] = [this](Event &e)
        {
            auto &event = static_cast<MouseReleaseEvent &>(e);
            utils::ServiceLocator::GetMouse()->OnEvent(event);
            //TORCH_LOG_DEBUG("Mouse Released: button {}", event.GetButton());
        };

        m_EventHandlers[EventType::MOUSE_MOVE] = [this](Event &e)
        {
            auto &event = static_cast<MouseMoveEvent &>(e);
            utils::ServiceLocator::GetMouse()->OnEvent(event);
            //TORCH_LOG_DEBUG("Mouse Moved: x = {}, y = {}", event.GetCursorPosX(), event.GetCursorPosY());
        };

        m_EventHandlers[EventType::MOUSE_SCROLL] = [this](Event &e)
        {
            auto &event = static_cast<MouseScrollEvent &>(e);
            utils::ServiceLocator::GetMouse()->OnEvent(event);
            //TORCH_LOG_DEBUG("Mouse scroll: x = {}, y = {}", event.GetOffsetX(), event.GetOffsetY());
        };

        m_EventHandlers[EventType::KEY_PRESS] = [this](Event &e)
        {
            auto &event = static_cast<KeyPressEvent &>(e);
            utils::ServiceLocator::GetKeyboard()->OnEvent(event);
            //TORCH_LOG_DEBUG("key presed: keycode = {}", event.GetKeyCode());
        };

        m_EventHandlers[EventType::KEY_RELEASE] = [this](Event &e)
        {
            auto &event = static_cast<KeyReleaseEvent &>(e);
            utils::ServiceLocator::GetKeyboard()->OnEvent(event);
            //TORCH_LOG_DEBUG("Key released: keycode = {}", event.GetKeyCode());
        };

        m_EventHandlers[EventType::KEY_REPEAT] = [this](Event &e)
        {
            auto &event = static_cast<KeyRepeatEvent &>(e);
            utils::ServiceLocator::GetKeyboard()->OnEvent(event);
            //TORCH_LOG_DEBUG("key repeated: keycode = {}", event.GetKeyCode());
        };

        m_EventHandlers[EventType::WINDOW_RESIZE] = [this](Event &e)
        {
            auto &event = static_cast<WindowResizeEvent &>(e);
            TORCH_LOG_DEBUG("Window Resized: width = {}, height = {}", event.GetWidth(), event.GetHeight());
        };
    }

    TorchWindow::~TorchWindow() noexcept
    {
        glfwDestroyWindow(m_Specification.glfwWindow);
        glfwTerminate();
    }

}