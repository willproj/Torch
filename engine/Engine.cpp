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
        Initialization();
    }

    Engine::~Engine()
    {
    }

    void Engine::Initialization()
    {
        TORCH_LOG_INFO("Torch Engine Initialized");
        std::unique_ptr<core::Window> window = core::Window::Create(
            core::WindowSpecification{ 800, 600, "My Window", nullptr }
        );
        TORCH_LOG_INFO("Torch Window created.");
        utils::ServiceLocator::RegisterWindow(std::move(window));
        TORCH_LOG_INFO("Torch Window registered.");


        //initialize graphics context
        TORCH_LOG_INFO("Initialize graphics context.");
        auto context = core::TorchGraphicsContext::GetGraphicsContext();

    }

    void Engine::operator()()
    {
        Run();
    }

    void Engine::Run()
    {
        auto appWindow = utils::ServiceLocator::GetWindow();
        while (!appWindow->ShouldClose())
        {
            appWindow->PollEvents();
            appWindow->SwapBuffers();
        }
    }
}
