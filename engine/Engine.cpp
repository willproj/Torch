// Engine.cpp
#include "Engine.h"

namespace engine
{
    std::unique_ptr<Engine> Engine::s_EngineInstance = nullptr;

    Engine& Engine::GetEngine()
    {
        if (!s_EngineInstance)
        {
            s_EngineInstance = std::unique_ptr<Engine>(new Engine());
        }
        return *s_EngineInstance;
    }

    Engine::Engine()
    {
        TORCH_LOG_INFO("Torch Engine Initialized: {0}", __FUNCTION__);
        std::unique_ptr<core::Window> window = core::Window::GetWindow(
            core::WindowSpecification{ 800, 600, "My Window", nullptr }
        );
        core::ServiceLocator::RegisterWindow(std::move(window));
    }

    Engine::~Engine()
    {
    }

    void Engine::operator()()
    {
        Run();
    }

    void Engine::Run()
    {
        auto appWindow = core::ServiceLocator::GetWindow();
        while (!appWindow->WindowShouldClose())
        {
            appWindow->PollEvents();
            appWindow->SwapBuffer();
        }
    }
}
