#pragma once
#include "EditorModule.h"


namespace editor
{
    class Viewport : public EditorModule
    {
    public:
        Viewport() = default;
        ~Viewport() = default;

        void Render() override;
        void OnUpdate() override;

        glm::vec2 GetWindowContentSize() const override;
        
    private: 
        glm::vec2 m_ViewportSize;
        glm::vec2 m_ScreenPixelPos;
        glm::vec2 viewportBounds[2];
    };
}