#include "Keyboard.h"

namespace core
{
    std::unique_ptr<Keyboard> Keyboard::s_Instance;

    Keyboard& Keyboard::GetInstance()
    {
        if (!s_Instance)
        {
            s_Instance = std::unique_ptr<Keyboard>(new Keyboard());
        }
        return *s_Instance;
    }

    void Keyboard::OnEvent(KeyPressEvent& event)
    {
        m_KeyStates[event.GetKeyCode()] = true; 
    }

    void Keyboard::OnEvent(KeyReleaseEvent& event)
    {
        m_KeyStates[event.GetKeyCode()] = false; 
    }

    void Keyboard::OnEvent(KeyRepeatEvent& event)
    {
        // key repeat count tracking
    }

    bool Keyboard::IsKeyPressed(int keyCode) const
    {
        auto it = m_KeyStates.find(keyCode);
        return it != m_KeyStates.end() && it->second;
    }
}
