#include "TorchWindow.h"

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

    TorchWindow::~TorchWindow() noexcept
    {
       glfwDestroyWindow(m_Specification.glfwWindow);
       glfwTerminate();
    }
	
}