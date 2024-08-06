#pragma once
#include <pch/pch.h>
namespace core
{

	struct WindowSpecification
	{
		uint32_t width;
		uint32_t height;
		std::string title;
		GLFWwindow* glfwWindow;
	};
	class Window
	{
	public:
		static std::unique_ptr<Window> GetWindow(const WindowSpecification& specification);

		virtual bool WindowShouldClose() = 0;
		virtual void PollEvents() =0;
		virtual void SwapBuffer() =0;
		virtual ~Window() = 0;
	protected:
		Window() = default;
		Window(const WindowSpecification& specification) : m_Specification(specification) {}
		WindowSpecification m_Specification;
	};
}