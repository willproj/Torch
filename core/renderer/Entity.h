#pragma once
#include <pch/pch.h>
#include <entt/entt.hpp>
#include "Component.h"

namespace tinygltf {
	class Model;
}

namespace core
{
	
	class Scene;
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity entity, Scene* scene, EntityType type);
		Entity(entt::entity entity, Scene* scene);
		~Entity() = default;

		Entity(const Entity&) = default;
		Entity& operator=(const Entity&) = default;


		template<typename Component, typename ...Args>
		bool AddComponent(Args... args)
		{
			if (HasComponent<Component>())
			{
				return false;
			}
			m_ScenePtr->GetRegisterRef().emplace<Component>(m_Entity, std::forward<Args>(args)...);
			return true;
		}

		template<typename Component>
		bool HasComponent()
		{
			return m_ScenePtr->GetRegisterRef().any_of<Component>(m_Entity);
		}

		template<typename Component>
		Component& GetComponent()
		{
			if (HasComponent<Component>())
			{
				return m_ScenePtr->GetRegisterRef().get<Component>(m_Entity);
			}
			return Component();
		}

		template<typename Component>
		bool RemoveComponent()
		{
			if (HasComponent<Component>())
			{
				m_ScenePtr->GetRegisterRef().remove<Component>(m_Entity);
				return true;
			}
			return false;
		}

		operator entt::entity() const { return m_Entity; }
		uint32_t operator()() { return static_cast<uint32_t>(m_Entity); }

		bool IsEntityEmpty() { return m_Entity == entt::null; }
	private:
		entt::entity m_Entity;
		Scene* m_ScenePtr;
		EntityType m_Type;
	};
}