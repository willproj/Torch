#pragma once
#include <pch/pch.h>
#include "EditorModule.h"
#include <core/graphics/TorchVulkanContext.h>
#include <core/platform/TorchWindow.h>

namespace editor
{
    enum class EditorType
    {
        None,
        Viewport,
        ScenePanel,
        Settings,
        EntityPanel,
        Scene,
        FileBrowser,
        Environment,
    };

    class Editor
    {
    public:
        Editor() = default;
        static void AddModule(EditorType type, std::unique_ptr<EditorModule> module);
        static void Render();
        static void SetUpImGui();
        static void ImGuiBegin();
        static void ImGuiEnd();
        static void OpenSceneFile();
        static void SaveSceneFile();
        static std::unique_ptr<EditorModule>& GetEditorModule(EditorType type) { return m_EditorModules[type]; }
    private:
        static void DockSpace();
        static void RenderNavbar();
        static std::unordered_map<EditorType, std::unique_ptr<EditorModule>> m_EditorModules;
        static core::Window* m_WindowPtr;

        static bool fullscreen;
        static bool isOpen;

        static uint32_t m_NavbarHeight;
        static bool m_FullScreen;
        static void KeyboardShortCut();
    };
}