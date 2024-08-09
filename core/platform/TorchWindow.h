#pragma once

#include "Window.h"
#include "core/events/WindowEvent.h"

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
        void SetWindowSize(uint32_t width, uint32_t height) noexcept override;
        void OnEvent(Event& event);

    private:
        TorchWindow() = delete;

        TorchWindow(const TorchWindow &) = delete;
        TorchWindow &operator=(const TorchWindow &) = delete;

        TorchWindow(TorchWindow &&) noexcept = default;
        TorchWindow &operator=(TorchWindow &&) noexcept = default;

    };
}
