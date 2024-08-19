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
    private: 
        
    };
}