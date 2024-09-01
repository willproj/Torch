#include "Scene.h"
#include "Component.h"
#include "Entity.h"
#include "ModelManager.h"

namespace core
{
	Entity Scene::CreateEntity(const std::string& entityName)
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

		entity.AddComponent<ModelComponent>();
		entity.AddComponent<TransformComponent>();

		return entity;
	}

	void Scene::SetSelectedEntityID(entt::entity entity)
	{
		m_SelectedEntityID = entity;
	}

	void Scene::RemoveEntity(Entity entity)
	{
	}

	entt::registry& Scene::GetRegisterRef()
	{
		return m_EnttRegistry;
	}
}