#include "SceneManager.h"
#include "Entity.h"
#include "Component.h"
#include "core/glcore/Shader.h"
#include "core/renderer/EnvironmentManager.h"
#include "core/renderer/ShaderManager.h"
#include "core/glcore/Texture.h"
#include "core/renderer/EnvironmentManager.h"
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

	void SceneManager::RenderScene()
	{
		auto& shadowMapDepthShader = ShaderManager::GetInstance()->GetShadowMapDepthShader();
		shadowMapDepthShader.use();

		for (auto entityID : m_Scene.GetGeneralEntityIDs())
		{
			Entity entity = { entityID, &m_Scene };

			// Get transform component
			glm::mat4 transform = glm::mat4(1.0f);
			if (entity.HasComponent<TransformComponent>())
			{
				transform = entity.GetComponent<TransformComponent>().getTransform();
			}

			shadowMapDepthShader.setMat4("model", transform);

			// Get material component
			if (entity.HasComponent<MaterialComponent>())
			{
				// Retrieve the material component from the entity
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

	void SceneManager::Render()
	{
		// Set up shader once before the loop
		auto& gemotryPassShader = ShaderManager::GetInstance()->GetGemotryPassShaderRef();

		gemotryPassShader.use();
		gemotryPassShader.setMat4("view", m_EditorCameraPtr->GetViewMatrix());
		gemotryPassShader.setMat4("projection", m_EditorCameraPtr->GetProjection());

		// Set up stencil buffer configuration before rendering
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);

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
			gemotryPassShader.setInt("entity", entity());
			gemotryPassShader.setMat4("model", transform);


			// Get material component
			if (entity.HasComponent<MaterialComponent>())
			{
				// Retrieve the material component from the entity
				auto& materialComponent = entity.GetComponent<MaterialComponent>();

				// Set up texture units and bind textures
				Texture::BindTexture(0, GL_TEXTURE_2D, materialComponent.albedoTexture);
				gemotryPassShader.setInt("u_AlbedoMap", 0); // Set shader uniform for albedo map
				gemotryPassShader.setBool("u_UseAlbedoMap", materialComponent.useAlbedoTexture); // Set shader uniform for albedo map
				
				Texture::BindTexture(1, GL_TEXTURE_2D, materialComponent.normalTexture);
				gemotryPassShader.setInt("u_NormalMap", 1); // Set shader uniform for normal map
				gemotryPassShader.setBool("u_UseNormalMap", materialComponent.useNormalTexture); // Set shader uniform for normal map

				Texture::BindTexture(2, GL_TEXTURE_2D, materialComponent.metallicTexture);
				gemotryPassShader.setInt("u_MetallicMap", 2); // Set shader uniform for metallic map
				gemotryPassShader.setBool("u_UseMetallicMap", materialComponent.useMetallicTexture); // Set shader uniform for metallic map
				gemotryPassShader.setFloat("u_Metallic", materialComponent.metallic);

				Texture::BindTexture(3, GL_TEXTURE_2D, materialComponent.roughnessTexture);
				gemotryPassShader.setInt("u_RoughnessMap", 3); // Set shader uniform for roughness map
				gemotryPassShader.setBool("u_UseRoughnessMap", materialComponent.useRoughnessTexture); // Set shader uniform for roughness map
				gemotryPassShader.setFloat("u_Roughness", materialComponent.roughness);

				Texture::BindTexture(4, GL_TEXTURE_2D, materialComponent.aoTexture);
				gemotryPassShader.setInt("u_AoMap", 4); // Set shader uniform for AO map
				gemotryPassShader.setBool("u_UseAoMap", materialComponent.useAOTexture); // Set shader uniform for AO map
			}
			else
			{
				gemotryPassShader.setBool("u_UseAlbedoMap", false);
				gemotryPassShader.setBool("u_UseNormalMap", false);
				gemotryPassShader.setBool("u_UseMetallicMap", false);
				gemotryPassShader.setBool("u_UseRoughness", false);
				gemotryPassShader.setBool("u_UseAoMap", false);
				
				gemotryPassShader.setFloat("u_Metallic", 0.0);
				gemotryPassShader.setFloat("u_Roughness", 1.0);
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
		glStencilMask(0x00);
		glDisable(GL_STENCIL_TEST);
	}

	void SceneManager::RenderLights()
	{
		// Set up shader once before the loop
		auto& lightsShader = ShaderManager::GetInstance()->GetLightsShader();
		lightsShader.use();
		lightsShader.setMat4("view", m_EditorCameraPtr->GetViewMatrix());
		lightsShader.setMat4("projection", m_EditorCameraPtr->GetProjection());

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
			lightsShader.setMat4("model", transform);

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
		auto& lightsIDShader = ShaderManager::GetInstance()->GetLightsIDShader();
		lightsIDShader.use();
		lightsIDShader.setMat4("view", m_EditorCameraPtr->GetViewMatrix());
		lightsIDShader.setMat4("projection", m_EditorCameraPtr->GetProjection());

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
			lightsIDShader.setInt("entity", entity());
			lightsIDShader.setMat4("model", transform);


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
		glEnable(GL_STENCIL_TEST); // Enable stencil test
		glStencilFunc(GL_EQUAL, 1, 0xFF); // Only render where stencil value is 1
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); // Don't change stencil value
		glStencilMask(0x00); // Disable writing to stencil buffer
		glDepthFunc(GL_LEQUAL);
		auto& outlineShader = ShaderManager::GetInstance()->GetStencilOutlineShaderRef();

		outlineShader.use();
		outlineShader.setMat4("view", m_EditorCameraPtr->GetViewMatrix());
		outlineShader.setMat4("projection", m_EditorCameraPtr->GetProjection());
		outlineShader.setVec3("outlineColor", glm::vec3(1.0f, 1.0f, 0.0f)); // Example outline color

		// Render slightly larger version of the objects
		for (auto entityID : m_Scene.GetGeneralEntityIDs())
		{
			Entity entity = { entityID, &m_Scene };

			glm::mat4 transform = glm::mat4(1.0f);
			if (entity.HasComponent<TransformComponent>())
			{
				transform = entity.GetComponent<TransformComponent>().getTransform();
			}

			// Scale the model slightly larger for the outline
			outlineShader.setMat4("model", glm::scale(transform, glm::vec3(1.05f))); // Slightly larger scale for the outline

			if (entity.HasComponent<ModelComponent>())
			{
				auto model = entity.GetComponent<ModelComponent>().model;
				if (model)
				{
					model->RenderModel(); // Render the model with the outline shader
				}
			}
		}

		glEnable(GL_DEPTH_TEST); // Re-enable depth testing
		glDisable(GL_STENCIL_TEST); // Disable stencil test
		glStencilMask(0xFF);        // Re-enable writing to the stencil buffer
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
	}





	SceneManager::SceneManager()
	{


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