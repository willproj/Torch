//platform/engine.h
#pragma once
#include <pch/pch.h>
#include "window/Window.h"
namespace engine
{
    class Engine
    {
    public:
        static Engine& GetEngine();
        ~Engine();
        void operator()();

    private:
        Engine();
        static std::unique_ptr<Engine> s_EngineInstance;
        void Run();

        std::unique_ptr<Window> m_TorchWindow;
    };
}