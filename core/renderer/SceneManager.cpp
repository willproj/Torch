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


			// Get material component
			if (entity.HasComponent<MaterialComponent>())
			{
				// Retrieve the material component from the entity
				auto& materialComponent = entity.GetComponent<MaterialComponent>();

				// Set up texture units and bind textures
				glActiveTexture(GL_TEXTURE0); // Activate texture unit 0
				glBindTexture(GL_TEXTURE_2D, materialComponent.albedoTexture); // Bind albedo texture
				m_GeomotryPassShader.setInt("u_AlbedoMap", 0); // Set shader uniform for albedo map

				glActiveTexture(GL_TEXTURE1); // Activate texture unit 1
				glBindTexture(GL_TEXTURE_2D, materialComponent.normalTexture); // Bind normal texture
				m_GeomotryPassShader.setInt("u_NormalMap", 1); // Set shader uniform for normal map

				glActiveTexture(GL_TEXTURE2); // Activate texture unit 2
				glBindTexture(GL_TEXTURE_2D, materialComponent.metallicTexture); // Bind metallic texture
				m_GeomotryPassShader.setInt("u_MetallicMap", 2); // Set shader uniform for metallic map

				glActiveTexture(GL_TEXTURE3); // Activate texture unit 3
				glBindTexture(GL_TEXTURE_2D, materialComponent.roughnessTexture); // Bind roughness texture
				m_GeomotryPassShader.setInt("u_RoughnessMap", 3); // Set shader uniform for roughness map

				glActiveTexture(GL_TEXTURE4); // Activate texture unit 4
				glBindTexture(GL_TEXTURE_2D, materialComponent.aoTexture); // Bind AO texture
				m_GeomotryPassShader.setInt("u_AoMap", 4); // Set shader uniform for AO map

				m_GeomotryPassShader.setFloat("u_Roughness", materialComponent.roughness);
				m_GeomotryPassShader.setFloat("u_Metallic", materialComponent.metallic);

			}

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


			// Get material component
			if (entity.HasComponent<MaterialComponent>())
			{
				auto& materialComponent = entity.GetComponent<MaterialComponent>();
				
			}

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