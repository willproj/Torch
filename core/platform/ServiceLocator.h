#pragma once
#include "pch/pch.h"
#include "Window.h"

namespace core
{
	class ServiceLocator
	{
	public:
        static void RegisterWindow(std::unique_ptr<Window> window);
		static Window* GetWindow();
        
	private:
		static std::unique_ptr<Window> s_Window;
	};
}