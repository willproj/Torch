#pragma once
#include <pch/pch.h>
#include <entt/entt.hpp>  

namespace core
{
    class Entity;
    class Scene
    {
    public:
        Scene() = default;
        ~Scene() = default;

        Entity CreateEntity(const std::string& entityName = "entity");
        void RemoveEntity(Entity entity);
        entt::registry& GetRegisterRef();
        
        void SetSelectedEntityID(entt::entity entity);
        void ResetSelectedEntityID() { m_SelectedEntityID = entt::null; }

        entt::entity GetSelectedEntityID() const { return m_SelectedEntityID; }

    private:
        entt::entity m_SelectedEntityID;
        entt::registry m_EnttRegistry;  
        friend class Entity;
    };
}
