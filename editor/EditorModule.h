#pragma once
#include <pch/pch.h>
#include <core/renderer/Entity.h>
namespace editor
{
    class EditorModule
    {
    public:
        EditorModule() = default;
        virtual ~EditorModule() = default;

        virtual void Render() = 0;
        virtual void OnUpdate() = 0;

        virtual glm::vec2 GetWindowContentSize() const { return glm::vec2(0.0f); }

    protected:
    };
}