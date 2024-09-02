#include "AtmosphericScattering.h"
#include "core/renderer/ModelManager.h"

namespace core
{
	void AtmosphericScattering::Initialize()
	{
		SetDefault();

		m_Type = EnvironmentEntityType::Atmosphere;
		m_Shader= Shader(
			std::string(PROJECT_ROOT) + "/assets/shader/skybox.vert",
			std::string(PROJECT_ROOT) + "/assets/shader/skybox.frag");
		
		const std::string filepath = std::string(PROJECT_ROOT) + "/assets/models/essential/sphere/scene.gltf";
		m_SkyboxSphere = ModelManager::GetInstance()->LoadModel(filepath);
	}

	void AtmosphericScattering::SetDefault()
	{
		auto& specific = std::get<AtmosphericScatteringSpecification>(m_Specification);
		specific.sunPosition = glm::normalize(glm::vec3(-1.0f, 1.0f, -1.0f));
		specific.sunIntensity = 1.5f;
		specific.rayTMin = 0.001f;
		specific.earthRadius= 6371000.0f;
		specific.atmosphereHeight= 100000.0f;
		specific.rayleighHeight= 8000.0f;
		specific.mieHeight = 1200.0f;
		specific.rayleighScatteringCoef= glm::vec3(5.802f, 13.558f, 33.1f) * 1e-6f;
		specific.mieScatteringCoef = glm::vec3(3.996) * 1e-6f;
		specific.ozoneAbsorptionCoef= glm::vec3(3.426f, 8.298f, 0.356f) * 0.1f * 1e-5f;
		specific.sunColor = glm::vec3(1.0f);
		specific.gamma = 2.8f;
		specific.exposure= 8.0f;
		specific.sunAngle = 2.0f * M_PI / 180.0f;
	}

	

	AtmosphericScattering::AtmosphericScattering(std::shared_ptr<EditorCamera> camera)
	{
		m_CurrentCamera = camera;
		Initialize();
	}

	void AtmosphericScattering::OnUpdate()
	{

	}

	void AtmosphericScattering::Render()
	{
		RenderAtmosphere();
	}

	void AtmosphericScattering::RenderAtmosphere()
	{
		auto& specific = std::get<AtmosphericScatteringSpecification>(m_Specification);
		glDepthFunc(GL_LEQUAL);
		m_Shader.use();
		m_Shader.setMat4("view", glm::mat4(glm::mat3(m_CurrentCamera->GetViewMatrix())));
		m_Shader.setMat4("projection", m_CurrentCamera->getProjection());
		m_Shader.setVec3("u_SunPosition", specific.sunPosition);
		m_Shader.setFloat("u_SunIntensity", specific.sunIntensity);
		m_Shader.setFloat("u_RayTMin", specific.rayTMin);
		m_Shader.setFloat("u_EarthRadius", specific.earthRadius);
		m_Shader.setFloat("u_AtmosphereHeight", specific.atmosphereHeight);
		m_Shader.setFloat("u_RayleighHeight", specific.rayleighHeight);
		m_Shader.setFloat("u_MieHeight", specific.mieHeight);
		m_Shader.setVec3("u_RayleighScatteringCoef", specific.rayleighScatteringCoef);
		m_Shader.setVec3("u_MieScatteringCoef", specific.mieScatteringCoef);
		m_Shader.setVec3("u_OzoneAbsorptionCoef", specific.ozoneAbsorptionCoef);
		m_Shader.setVec3("u_SunColor", specific.sunColor);
		m_Shader.setFloat("u_SunAngle", specific.sunAngle);
		m_SkyboxSphere->RenderModel();
		glDepthFunc(GL_LESS);
	}
}