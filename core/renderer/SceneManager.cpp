#include "SceneManager.h"
#include "Entity.h"
#include "Component.h"
#include "core/glcore/Shader.h"

namespace core
{

	std::shared_ptr<SceneManager> SceneManager::m_SceneManager = nullptr;

	void SceneManager::CreateEntity()
	{
		m_Scene.CreateEntity("new entity");
	}

	void SceneManager::RemoveEntity(Entity entity)
	{
		m_Scene.RemoveEntity(entity);
	}

	std::shared_ptr<SceneManager> SceneManager::GetSceneManager()
	{
		if (!m_SceneManager)
		{
			m_SceneManager = std::shared_ptr<SceneManager>(new SceneManager());
		}
		return m_SceneManager;
	}

	void SceneManager::Render(Shader& shader)
	{
		auto& view = m_Scene.GetRegisterRef().view<entt::entity>();
		for (auto& entityID : view)
		{
			//init entity
			shader.setInt("entity", 2);
			Entity entity = { entityID, &m_Scene };
			//render model
			entity.GetComponent<ModelComponent>().model->RenderModel();
		}
	}

	SceneManager::SceneManager()
	{
	}

	SceneManager::~SceneManager()
	{
	}
}