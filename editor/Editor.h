#pragma once
#include <pch/pch.h>
#include "EditorModule.h"
#include <core/graphics/TorchVulkanContext.h>
#include <core/platform/TorchWindow.h>

namespace editor
{
    class Editor
    {
    public:
        Editor() = default;
        void AddModule(std::unique_ptr<EditorModule> module);
        void Render();
        static void SetUpImGui();
        static void ImGuiBegin();
        static void ImGuiEnd();
    private:
        std::vector<std::unique_ptr<EditorModule>> m_EditorModules;
        static core::Window* m_WindowPtr;
    };
}