#pragma once

#include "Window.h"
#include "core/events/Event.h"
#include "core/events/EventDispatcher.h"

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
        void ResetIsResize() noexcept override;
        bool IsResize() noexcept override { return m_IsResized; }
        void HandleMinimization() noexcept override;
    private:

        using EventHandler = std::function<void(Event&)>;
        std::unordered_map<EventType, EventHandler> m_EventHandlers;
        bool m_IsResized = false;

        void RegisteEventHandlers();

        TorchWindow() = delete;

        TorchWindow(const TorchWindow &) = delete;
        TorchWindow &operator=(const TorchWindow &) = delete;

        TorchWindow(TorchWindow &&) noexcept = default;
        TorchWindow &operator=(TorchWindow &&) noexcept = default;

    };
}
