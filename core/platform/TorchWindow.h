#pragma once

#include "Window.h"

namespace core
{
    class TorchWindow : public Window
    {
    public:
        explicit TorchWindow(const WindowSpecification &specification);
        ~TorchWindow() noexcept override;

        bool ShouldClose() const noexcept override;
        void PollEvents() noexcept override;
        void SwapBuffers() noexcept override;

    private:
        TorchWindow() = delete;

        TorchWindow(const TorchWindow &) = delete;
        TorchWindow &operator=(const TorchWindow &) = delete;

        TorchWindow(TorchWindow &&) noexcept = default;
        TorchWindow &operator=(TorchWindow &&) noexcept = default;
    };
}
