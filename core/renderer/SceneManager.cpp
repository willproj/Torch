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

	void SceneManager::Render()
	{
		auto& view = m_Scene.GetRegisterRef().view<entt::entity>();
		for (auto& entityID : view)
		{
			//set up shader
			m_Shader.use();
			m_Shader.setInt("entity", static_cast<uint32_t>(entityID));
			m_Shader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -2.0f, -2.0f)));
			m_Shader.setMat4("view", m_EditorCameraPtr->GetViewMatrix());
			m_Shader.setMat4("projection", m_EditorCameraPtr->getProjection());
			//init entity
			Entity entity = { entityID, &m_Scene };
			//render model
			if (entity.HasComponent<ModelComponent>())
			{
				auto model = entity.GetComponent<ModelComponent>().model;
				if (model)
				{
					model->RenderModel();
				}
			}
		}
	}

	SceneManager::SceneManager()
	{
		m_Shader = Shader(
			std::string(PROJECT_ROOT) + "/assets/shader/shader.vert",
			std::string(PROJECT_ROOT) + "/assets/shader/shader.frag");
	}

	SceneManager::~SceneManager()
	{
	}

	void SceneManager::SetCamera(std::shared_ptr<EditorCamera> camera)
	{
		m_EditorCameraPtr = camera;
	}
}