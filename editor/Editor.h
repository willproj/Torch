#pragma once
#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>
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

        static ImGui_ImplVulkanH_Window m_MainWindowData;
        static int m_MinImageCount;
        static bool m_SwapChainRebuild;
        static void FrameRender(ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data);
        static void FramePresent(ImGui_ImplVulkanH_Window* wd);

        static core::TorchVulkanContext* m_GraphicsContextPtr;
        static core::Window* m_WindowPtr;
    };
}