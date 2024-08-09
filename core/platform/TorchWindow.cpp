#include "TorchWindow.h"
#include "core/events/EventDispatcher.h"

namespace core
{
    TorchWindow::TorchWindow(const WindowSpecification& specification)
        : Window(specification) 
    {
        if (!glfwInit())
        {
            TORCH_LOG_ERROR("Failed to initialzie GLFW.");
            return;
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_Specification.glfwWindow = glfwCreateWindow(m_Specification.width, m_Specification.height, m_Specification.title.c_str(), nullptr, nullptr);
        if (!m_Specification.glfwWindow)
        {
            TORCH_LOG_ERROR("Failed to create GLFW window.");
            return;
        }
        glfwMakeContextCurrent(m_Specification.glfwWindow);

        glfwSetWindowUserPointer(m_Specification.glfwWindow, this);

        //set window size call back function
        glfwSetWindowSizeCallback(m_Specification.glfwWindow, [](GLFWwindow* window, int width, int height)
        {
            auto* torchWindow = static_cast<TorchWindow*>(glfwGetWindowUserPointer(window));
            torchWindow->SetWindowSize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));

            WindowResizeEvent event(width, height);
            torchWindow->OnEvent(event);
        });


        //set window close callback
        glfwSetWindowCloseCallback(m_Specification.glfwWindow, [](GLFWwindow* window)
        {
            auto* torchWindow = static_cast<TorchWindow*>(glfwGetWindowUserPointer(window));
            TORCH_LOG_DEBUG("[{}:{}] window closed.", __FILE__, __LINE__);

            bool flag = true;
            if (!flag)
            {
                glfwSetWindowShouldClose(window, GLFW_FALSE);
            }
        });
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
    }

    void TorchWindow::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<WindowResizeEvent>([](WindowResizeEvent& e)
        {
            TORCH_LOG_DEBUG("[{}:{}] --window size: {}x{}", __FILE__, __LINE__, e.GetWidth(), e.GetHeight());
            return true;
        });

    }

    TorchWindow::~TorchWindow() noexcept
    {
       glfwDestroyWindow(m_Specification.glfwWindow);
       glfwTerminate();
    }
	
}