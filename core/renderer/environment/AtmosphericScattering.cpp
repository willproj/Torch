#include "AtmosphericScattering.h"
#include "core/renderer/ModelManager.h"

namespace core
{
	void AtmosphericScattering::Initialize()
	{
		m_Type = EnvironmentEntityType::Atmosphere;
		m_Shader= Shader(
			std::string(PROJECT_ROOT) + "/assets/shader/skybox.vert",
			std::string(PROJECT_ROOT) + "/assets/shader/skybox.frag");

		const std::string filepath = std::string(PROJECT_ROOT) + "/assets/models/essential/sphere/scene.gltf";
		m_SkyboxSphere = ModelManager::GetInstance()->LoadModel(filepath);
	}

	AtmosphericScattering::AtmosphericScattering(std::shared_ptr<EditorCamera> camera)
	{
		m_CurrentCamera = camera;
		Initialize();
	}

	void AtmosphericScattering::Render()
	{
		glDepthFunc(GL_LEQUAL);
		m_Shader.use();
		m_Shader.setMat4("view", glm::mat4(glm::mat3(m_CurrentCamera->GetViewMatrix())));
		m_Shader.setMat4("projection", m_CurrentCamera->getProjection());
		m_Shader.setVec3("u_SunDir", glm::normalize(std::get<AtmosphericScatteringSpecification>(m_Specification).sunDir));
		m_SkyboxSphere->RenderModel();
		glDepthFunc(GL_LESS);
	}
}