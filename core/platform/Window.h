#pragma once

#include <pch/pch.h>

namespace core
{
	struct WindowSpecification
	{
		uint32_t width = 800;                  // Default width
		uint32_t height = 600;                 // Default height
		std::string title = "Default Window";  // Default title
		GLFWwindow* glfwWindow = nullptr;     // Default nullptr
	};

	class Window
	{
	public:
		static std::unique_ptr<Window> Create(const WindowSpecification& specification);

		virtual bool ShouldClose() const noexcept = 0;
		virtual void PollEvents() noexcept = 0;
		virtual void SwapBuffers() noexcept = 0;
		virtual ~Window() noexcept = 0;
		virtual void SetWindowSize(uint32_t width, uint32_t height) noexcept = 0;
		WindowSpecification GetWinSpecification() const { return m_Specification; }
		virtual void ResetIsResize() noexcept {};
		virtual bool IsResize() noexcept { return false; };
		virtual void HandleMinimization() noexcept {};
	protected:
		Window() = default;
		explicit Window(const WindowSpecification& specification) : m_Specification(specification) {}
		WindowSpecification m_Specification;

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		// Allow move operations
		Window(Window&&) noexcept = default;
		Window& operator=(Window&&) noexcept = default;
	};
}
