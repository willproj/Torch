#pragma once
#include <pch/pch.h>
#include <entt/entt.hpp>  
#include "Entity.h"

namespace core
{
   

    class Entity; // Forward declaration
    class Scene
    {
    public:
        Scene() = default;
        ~Scene() = default;

        Scene(Scene&& other) noexcept;
        Scene& operator=(Scene&& other) noexcept;

        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        Entity CreateEntity(const std::string& entityName = "entity", EntityType type = EntityType::General);
        
        void RemoveEntity(Entity entity);

        entt::registry& GetRegisterRef() { return m_EnttRegistry; }

        void SetSelectedEntityID(entt::entity entity) { m_SelectedEntityID = entity; }

        void ResetSelectedEntityID() { m_SelectedEntityID = entt::null; }

        entt::entity GetSelectedEntityID() const { return m_SelectedEntityID; }

        const std::vector<entt::entity>& GetGeneralEntityIDs() const { return m_GeneralEntityIDs; }
        const std::vector<entt::entity>& GetLightEntityIDs() const { return m_LightEntityIDs; }

    private:
        entt::entity m_SelectedEntityID = entt::null; 
        entt::registry m_EnttRegistry;
        friend class Entity; 

        std::vector<entt::entity> m_GeneralEntityIDs;
        std::vector<entt::entity> m_LightEntityIDs;
    };
}
