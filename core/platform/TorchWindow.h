#pragma once
#include "Window.h"

namespace core
{
    class TorchWindow : public Window
    {
    public:
        bool WindowShouldClose() override;
        void PollEvents() override;
        void SwapBuffer() override;
        explicit TorchWindow(const WindowSpecification& specification);
        ~TorchWindow() override;

    private:
        TorchWindow() = delete; 
    };
}