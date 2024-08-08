#pragma once

#include <pch/pch.h>
#include <core/platform/Window.h>

namespace utils
{
    class ServiceLocator
    {
    public:
        static void RegisterWindow(std::unique_ptr<core::Window> window);

        static core::Window* GetWindow();

    private:
        static std::optional<std::unique_ptr<core::Window>> s_Window;
        static std::mutex s_WindowMutex;
    };
}
