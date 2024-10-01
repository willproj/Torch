#include "SceneManager.h"
#include "Entity.h"
#include "Component.h"
#include "core/glcore/Shader.h"
#include "core/renderer/EnvironmentManager.h"
#include "core/renderer/ShaderManager.h"
#include "core/glcore/Texture.h"
#include "core/renderer/EnvironmentManager.h"
#include <utils/ServiceLocator.h>

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

		for (auto entityID : m_Scene.GetRegisterRef().view<entt::entity>())
		{
			Entity entity = { entityID, &m_Scene };

			// Get component type
			if (entity.HasComponent<EntityTypeComponent>())
			{
				auto& type = entity.GetComponent<EntityTypeComponent>().entityType;

				//if it is lights then ignore the shadow
				if (type == EntityType::Light)
				{
					continue;
				}
			}

			// Get transform component
			glm::mat4 transform = glm::mat4(1.0f);
			if (entity.HasComponent<TransformComponent>())
			{
				transform = entity.GetComponent<TransformComponent>().getTransform();
			}

			shadowMapDepthShader.setMat4("model", transform);

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
		// Iterate over entities and render
		for (auto entityID : m_Scene.GetRegisterRef().view<entt::entity>())
		{
			Entity entity = { entityID, &m_Scene };

			// Get Type component
			EntityType type;
			if (entity.HasComponent<EntityTypeComponent>())
			{
				type = entity.GetComponent<EntityTypeComponent>().entityType;
			}

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
	}

	SceneManager::SceneManager()
	{
	}


	SceneManager::~SceneManager()
	{
	}

	void SceneManager::SetCamera(std::shared_ptr<EditorCamera> camera)
	{
		m_EditorCameraPtr = camera;
	}
}