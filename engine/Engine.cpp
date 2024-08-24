#include "Engine.h"
#include "editor/Editor.h"

namespace engine
{
    std::unique_ptr<Engine> Engine::s_EngineInstance = nullptr;

    Engine &Engine::GetEngine()
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

    void Engine::Render()
    {
    }

    void Engine::Initialization()
    {
        std::unique_ptr<core::Window> window = core::Window::Create(
            core::WindowSpecification{1280, 720, "My Window", nullptr});
        utils::ServiceLocator::RegisterWindow(std::move(window));
        utils::ServiceLocator::RegisterGraphicsContext(std::move(core::TorchGraphicsContext::GetGraphicsContext()));
        editor::Editor::SetUpImGui();
        editor::Editor::AddModule(std::make_unique<editor::Viewport>());


        TORCH_LOG_INFO("Torch Engine Initialized");
    }

    void Engine::operator()()
    {
        Run();
    }

    void Engine::Run()
    {
        auto appWindow = utils::ServiceLocator::GetWindow();
        auto context = utils::ServiceLocator::GetGraphicsContext();
        while (!appWindow->ShouldClose())
        {
            appWindow->PollEvents();

            if (appWindow->IsResize())
            {
                appWindow->HandleMinimization();
                appWindow->ResetIsResize();
            }

            context->DrawFrame();
            editor::Editor::ImGuiBegin();
            editor::Editor::Render();
            editor::Editor::ImGuiEnd();


            appWindow->SwapBuffers();
        }
    }
}
