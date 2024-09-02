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

        // Move constructor
        Scene(Scene&& other) noexcept;

        // Move assignment operator
        Scene& operator=(Scene&& other) noexcept;

        // Deleted copy constructor and copy assignment operator
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        // Create a new entity and return it
        Entity CreateEntity(const std::string& entityName = "entity", EntityType type = EntityType::General);
        
        // Remove an entity from the scene
        void RemoveEntity(Entity entity);

        // Get a reference to the entity registry
        entt::registry& GetRegisterRef() { return m_EnttRegistry; }

        // Set the currently selected entity
        void SetSelectedEntityID(entt::entity entity) { m_SelectedEntityID = entity; }

        // Reset the selected entity to null
        void ResetSelectedEntityID() { m_SelectedEntityID = entt::null; }

        // Get the currently selected entity ID
        entt::entity GetSelectedEntityID() const { return m_SelectedEntityID; }

    private:
        entt::entity m_SelectedEntityID = entt::null; // ID of the currently selected entity
        entt::registry m_EnttRegistry; // Registry holding all entities
        friend class Entity; // Grant Entity class access to private members
    };
}
