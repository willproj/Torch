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

	void SceneManager::SetScene(Scene&& scene)
	{
		m_Scene = std::move(scene);
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
			Entity entity = { entityID, &m_Scene };
			//get transform component
			glm::mat4 transform = glm::mat4(1.0f);
			if (entity.HasComponent<TransformComponent>())
			{
				transform = entity.GetComponent<TransformComponent>().getTransform();
			}

			//set up shader
			m_Shader.use();
			m_Shader.setInt("entity", static_cast<uint32_t>(entityID));
			m_Shader.setMat4("model", transform);
			m_Shader.setMat4("view", m_EditorCameraPtr->GetViewMatrix());
			m_Shader.setMat4("projection", m_EditorCameraPtr->getProjection());
			//init entity
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

	void SceneManager::StencilOutlinePass()
	{
		glEnable(GL_STENCIL_TEST);
		glClear(GL_STENCIL_BUFFER_BIT);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		auto& view = m_Scene.GetRegisterRef().view<entt::entity>();
		for (auto& entityID : view)
		{
			Entity entity = { entityID, &m_Scene };

			// Get transform component
			if (!entity.HasComponent<TransformComponent>()) continue;
			TransformComponent transformComponent = entity.GetComponent<TransformComponent>();

			// Render the model into stencil buffer (not into color buffer)
			if (entity.HasComponent<ModelComponent>())
			{
				auto model = entity.GetComponent<ModelComponent>().model;
				if (model)
				{
					m_StencilOutlineShader.use();
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), transformComponent.translation)
						* glm::toMat4(glm::quat(transformComponent.rotation))
						* glm::scale(glm::mat4(1.0f), 2.0f * transformComponent.scale);

					m_StencilOutlineShader.setMat4("model", transform);
					m_StencilOutlineShader.setMat4("view", m_EditorCameraPtr->GetViewMatrix());
					m_StencilOutlineShader.setMat4("projection", m_EditorCameraPtr->getProjection());

					model->RenderModel();
				}
			}
		}
	}



	SceneManager::SceneManager()
	{
		m_Shader = Shader(
			std::string(PROJECT_ROOT) + "/assets/shader/shader.vert",
			std::string(PROJECT_ROOT) + "/assets/shader/shader.frag"
		);

		m_StencilOutlineShader = Shader(
			std::string(PROJECT_ROOT) + "/assets/shader/outline.vert",
			std::string(PROJECT_ROOT) + "/assets/shader/outline.frag"
		);
	}

	SceneManager::~SceneManager()
	{
	}

	void SceneManager::SetCamera(std::shared_ptr<EditorCamera> camera)
	{
		m_EditorCameraPtr = camera;
	}
}