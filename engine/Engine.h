//platform/engine.h
#pragma once
#include <pch/pch.h>
#include "Headers.h"

namespace engine
{
    class TorchGraphicsContext;
    class Engine
    {
    public:
        static Engine& GetEngine();
        ~Engine();
        void operator()();

        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;

        Engine(Engine&&) = delete;
        Engine& operator=(Engine&&) = delete;

    private:
        Engine();
        void Run();
        void Render();
        void Initialization();
        static std::unique_ptr<Engine> s_EngineInstance;
        
    };
}