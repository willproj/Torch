#include "Scene.h"
#include "Component.h"
#include "Entity.h"
#include "ModelManager.h"

namespace core
{
	Scene::Scene(Scene&& other) noexcept
		: m_SelectedEntityID(other.m_SelectedEntityID), m_EnttRegistry(std::move(other.m_EnttRegistry))
	{

	}

	Scene& Scene::operator=(Scene&& other) noexcept
	{
		if (this != &other)
		{
			m_SelectedEntityID = other.m_SelectedEntityID;

			m_EnttRegistry = std::move(other.m_EnttRegistry);
			m_GeneralEntityIDs = std::move(other.m_GeneralEntityIDs);

			other.m_SelectedEntityID = entt::null;
		}
		return *this;
	}


	//create general entity
	Entity Scene::CreateEntity(const std::string& entityName, EntityType type)
	{
		Entity entity(m_EnttRegistry.create(), this);

		//init uuid for entity
		TORCH_LOG_DEBUG("[{},{}] Add UUID to Entity", __FILE__, __LINE__);
		utils::UUID uuid;
		entity.AddComponent<UUIDComponent>();
		auto& uuidComponent = entity.GetComponent<UUIDComponent>();
		uuidComponent.uuid = uuid;

		//init label for entity
		TORCH_LOG_DEBUG("[{},{}] Add label to Entity", __FILE__, __LINE__);
		entity.AddComponent<LabelComponent>();
		auto& labelComponent = entity.GetComponent<LabelComponent>();
		labelComponent.label = entityName;

		entity.AddComponent<EntityTypeComponent>();
		auto& typeComponent = entity.GetComponent<EntityTypeComponent>();
		typeComponent.entityType = type;

		entity.AddComponent<ModelComponent>();
		entity.AddComponent<TransformComponent>();

		return entity;
	}

	void Scene::RemoveEntity(Entity entity)
	{
	}
	
}