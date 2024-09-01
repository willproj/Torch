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
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_STENCIL_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST); // Disable depth test for outline rendering

		auto& view = m_Scene.GetRegisterRef().view<entt::entity>();
		for (auto& entityID : view)
		{
			Entity entity = { entityID, &m_Scene };
			// Get transform component
			TransformComponent transformComponent;
			if (entity.HasComponent<TransformComponent>())
			{
				transformComponent = entity.GetComponent<TransformComponent>();
			}

			// Create a modified scale for the outline
			float outlineScale = 2.1f; // Adjust this value to control outline thickness
			glm::vec3 outlineScaleVec = transformComponent.scale * outlineScale;

			// Create a modified transform matrix for the outline
			glm::mat4 outlineTransform = glm::translate(glm::mat4(1.0f), transformComponent.translation)
				* glm::toMat4(glm::quat(transformComponent.rotation))
				* glm::scale(glm::mat4(1.0f), outlineScaleVec);

			// Set up shader
			glm::mat4 simpleTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
			m_StencilOutlineShader.use();
			m_StencilOutlineShader.setMat4("model", simpleTransform);
			m_StencilOutlineShader.setMat4("view", m_EditorCameraPtr->GetViewMatrix());
			m_StencilOutlineShader.setMat4("projection", m_EditorCameraPtr->getProjection());

			// Render model
			if (entity.HasComponent<ModelComponent>())
			{
				auto model = entity.GetComponent<ModelComponent>().model;
				if (model)
				{
					model->RenderModel();
				}
			}
		}

		glDisable(GL_STENCIL_TEST);
		glEnable(GL_DEPTH_TEST);
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