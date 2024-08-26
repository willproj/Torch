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

		//init model for entity
		TORCH_LOG_DEBUG("[{},{}] Add model to Entity", __FILE__, __LINE__);
		entity.AddComponent<ModelComponent>();
		auto& modelComponent = entity.GetComponent<ModelComponent>();
		auto& modelManager = ModelManager::GetInstance();
		const std::string filepath = "/assets/models/sphere/sphere.gltf";
		modelManager->LoadModel(filepath);
		modelComponent.model = modelManager->GetModel(filepath);
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