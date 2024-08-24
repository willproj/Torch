#pragma once
#include <pch/pch.h>
#include "EditorModule.h"
#include <core/graphics/TorchVulkanContext.h>
#include <core/platform/TorchWindow.h>
#include "Viewport.h"

namespace editor
{
    class Editor
    {
    public:
        Editor() = default;
        static void AddModule(std::unique_ptr<EditorModule> module);
        static void Render();
        static void SetUpImGui();
        static void ImGuiBegin();
        static void ImGuiEnd();
    private:
        static std::vector<std::unique_ptr<EditorModule>> m_EditorModules;
        static core::Window* m_WindowPtr;

    };
}