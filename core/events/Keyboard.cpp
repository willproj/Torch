#include "Keyboard.h"

namespace core
{
    std::unique_ptr<Keyboard> Keyboard::s_Instance;

    std::unique_ptr<Keyboard>& Keyboard::GetInstance()
    {
        if (!s_Instance)
        {
            s_Instance = std::unique_ptr<Keyboard>(new Keyboard());
        }
        return s_Instance;
    }

    void Keyboard::OnEvent(KeyPressEvent& event)
    {
        m_KeyStates[event.GetKeyCode()] = true; 
    }

    void Keyboard::OnEvent(KeyReleaseEvent& event)
    {
        std::cout << "released" << std::endl;
        m_KeyStates[event.GetKeyCode()] = false; 
    }

    void Keyboard::OnEvent(KeyRepeatEvent& event)
    {
        // key repeat count tracking
    }

    bool Keyboard::IsKeyPressed(KeyCode keyCode) const
    {
        int32_t code = static_cast<int32_t>(keyCode); 
        auto it = m_KeyStates.find(code);
        return it != m_KeyStates.end() && it->second;
    }
}
