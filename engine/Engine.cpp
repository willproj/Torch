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
        WindowSpecification spec{ 800, 600, "Torch Engine", nullptr };
        m_TorchWindow = Window::GetWindow(spec);
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
        TORCH_LOG_INFO("Engine is going to run: {}", !m_TorchWindow->WindowShouldClose());
        while (!m_TorchWindow->WindowShouldClose())  
        {
            m_TorchWindow->PollEvents();
            m_TorchWindow->SwapBuffer();
        }
        TORCH_LOG_INFO("Engine is stopped: {}", !m_TorchWindow->WindowShouldClose());
    }
}
