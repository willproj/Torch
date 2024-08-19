#pragma once
#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

namespace editor
{
    class EditorModule
    {
    public:
        EditorModule() = default;
        virtual ~EditorModule() = default;

        virtual void Render() = 0;
        virtual void OnUpdate() = 0;
    };
}