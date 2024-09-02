#include "SceneManager.h"
#include "Entity.h"
#include "Component.h"
#include "core/glcore/Shader.h"

namespace core
{

	std::shared_ptr<SceneManager> SceneManager::m_SceneManager = nullptr;

	void SceneManager::CreateEntity(EntityType type)
	{
		m_Scene.CreateEntity("new entity", type);
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
		// Set up shader once before the loop
		m_GeomotryPassShader.use();
		m_GeomotryPassShader.setMat4("view", m_EditorCameraPtr->GetViewMatrix());
		m_GeomotryPassShader.setMat4("projection", m_EditorCameraPtr->getProjection());

		// Iterate over entities and render
		for (auto entityID : m_Scene.GetGeneralEntityIDs())
		{
			Entity entity = { entityID, &m_Scene };

			// Get transform component
			glm::mat4 transform = glm::mat4(1.0f);
			if (entity.HasComponent<TransformComponent>())
			{
				transform = entity.GetComponent<TransformComponent>().getTransform();
			}
			// Set entity-specific shader uniforms
			m_GeomotryPassShader.setInt("entity", entity());
			m_GeomotryPassShader.setMat4("model", transform);

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
	}

	void SceneManager::RenderLights()
	{
		// Set up shader once before the loop
		m_LightsShader.use();
		m_LightsShader.setMat4("view", m_EditorCameraPtr->GetViewMatrix());
		m_LightsShader.setMat4("projection", m_EditorCameraPtr->getProjection());

		// Iterate over entities and render
		for (auto entityID : m_Scene.GetLightEntityIDs())
		{
			Entity entity = { entityID, &m_Scene };

			// Get transform component
			glm::mat4 transform = glm::mat4(1.0f);
			if (entity.HasComponent<TransformComponent>())
			{
				transform = entity.GetComponent<TransformComponent>().getTransform();
			}
			// Set entity-specific shader uniforms
			m_LightsShader.setMat4("model", transform);

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
	}


	void SceneManager::RenderLightsToID()
	{
		// Set up shader once before the loop
		m_LightsIDShader.use();
		m_LightsIDShader.setMat4("view", m_EditorCameraPtr->GetViewMatrix());
		m_LightsIDShader.setMat4("projection", m_EditorCameraPtr->getProjection());

		// Iterate over entities and render
		for (auto entityID : m_Scene.GetLightEntityIDs())
		{
			Entity entity = { entityID, &m_Scene };

			// Get transform component
			glm::mat4 transform = glm::mat4(1.0f);
			if (entity.HasComponent<TransformComponent>())
			{
				transform = entity.GetComponent<TransformComponent>().getTransform();
			}
			// Set entity-specific shader uniforms
			m_LightsIDShader.setInt("entity", entity());
			m_LightsIDShader.setMat4("model", transform);

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
	}


	void SceneManager::StencilOutlinePass()
	{
		
	}



	SceneManager::SceneManager()
	{
		m_GeomotryPassShader = Shader(
			std::string(PROJECT_ROOT) + "/assets/shader/shader.vert",
			std::string(PROJECT_ROOT) + "/assets/shader/shader.frag"
		);

		m_LightsShader = Shader(
			std::string(PROJECT_ROOT) + "/assets/shader/lightShader.vert",
			std::string(PROJECT_ROOT) + "/assets/shader/lightShader.frag"
		);

		m_LightsIDShader = Shader(
			std::string(PROJECT_ROOT) + "/assets/shader/lightShader.vert",
			std::string(PROJECT_ROOT) + "/assets/shader/lightIDShader.frag"
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