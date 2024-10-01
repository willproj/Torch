#include "AtmosphericScattering.h"
#include "core/renderer/ModelManager.h"
#include "core/renderer/RenderCube.h"
#include "core/renderer/ShaderManager.h"

namespace core
{
	void AtmosphericScattering::Initialize()
	{
		SetDefault();

		m_Type = EnvironmentEntityType::Atmosphere;
		const std::string filepath = std::string(PROJECT_ROOT) + "/assets/models/essential/sphere/scene.gltf";
		m_SkyboxSphere = ModelManager::GetInstance()->LoadModel(filepath);

		// Define a buffer ID
		glGenBuffers(1, &ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

		// Allocate memory for the SSBO (size of vec3, which is 3 floats, 3 * sizeof(float))
		glBufferData(GL_SHADER_STORAGE_BUFFER, 3 * sizeof(float), NULL, GL_DYNAMIC_COPY);

		// Bind the SSBO to binding point 0, matching the layout in the shader
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

		// Unbind the buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	void AtmosphericScattering::SetDefault()
	{
		auto& specific = std::get<AtmosphericScatteringSpecification>(m_Specification);
		specific.sunPosition = glm::normalize(glm::vec3(-1.0f, 1.0f, -1.0f));
		specific.sunIntensity = 2.0f;
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
		specific.sunAngle = 0.0f * M_PI / 180.0f;
	}

	void AtmosphericScattering::SetIsRunning(bool isRunning)
	{
		m_IsRunning = isRunning;
	}
	
	bool AtmosphericScattering::IsRunning()
	{
		return m_IsRunning;
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

	void AtmosphericScattering::SetShader()
	{
		auto& shader = ShaderManager::GetInstance()->GetAtmosphereShaderRef();
		auto& specific = std::get<AtmosphericScatteringSpecification>(m_Specification);
		shader.use();
		shader.setVec3("u_SunPosition", specific.sunPosition);
		shader.setFloat("u_SunIntensity", specific.sunIntensity);
		shader.setFloat("u_RayTMin", specific.rayTMin);
		shader.setFloat("u_EarthRadius", specific.earthRadius);
		shader.setFloat("u_AtmosphereHeight", specific.atmosphereHeight);
		shader.setFloat("u_RayleighHeight", specific.rayleighHeight);
		shader.setFloat("u_MieHeight", specific.mieHeight);
		shader.setVec3("u_RayleighScatteringCoef", specific.rayleighScatteringCoef);
		shader.setVec3("u_MieScatteringCoef", specific.mieScatteringCoef);
		shader.setVec3("u_OzoneAbsorptionCoef", specific.ozoneAbsorptionCoef);
		shader.setVec3("u_SunColor", specific.sunColor);
		shader.setFloat("u_SunAngle", specific.sunAngle);
	}

	void AtmosphericScattering::Render(const glm::mat4& view, const glm::mat4& projection)
	{
		auto& shader = ShaderManager::GetInstance()->GetAtmosphereShaderRef();
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		glDepthFunc(GL_LEQUAL);
		RenderCube::Render();
		glDepthFunc(GL_LESS);
	}

	void AtmosphericScattering::RenderAtmosphere()
	{
		auto& shader = ShaderManager::GetInstance()->GetAtmosphereShaderRef();
		auto& specific = std::get<AtmosphericScatteringSpecification>(m_Specification);
		glDepthFunc(GL_LEQUAL);
		shader.use();
		shader.setMat4("view", glm::mat4(glm::mat3(m_CurrentCamera->GetViewMatrix())));
		shader.setMat4("projection", m_CurrentCamera->GetProjection());
		shader.setVec3("u_SunPosition", specific.sunPosition);
		shader.setFloat("u_SunIntensity", specific.sunIntensity);
		shader.setFloat("u_RayTMin", specific.rayTMin);
		shader.setFloat("u_EarthRadius", specific.earthRadius);
		shader.setFloat("u_AtmosphereHeight", specific.atmosphereHeight);
		shader.setFloat("u_RayleighHeight", specific.rayleighHeight);
		shader.setFloat("u_MieHeight", specific.mieHeight);
		shader.setVec3("u_RayleighScatteringCoef", specific.rayleighScatteringCoef);
		shader.setVec3("u_MieScatteringCoef", specific.mieScatteringCoef);
		shader.setVec3("u_OzoneAbsorptionCoef", specific.ozoneAbsorptionCoef);
		shader.setVec3("u_SunColor", specific.sunColor);
		shader.setFloat("u_SunAngle", specific.sunAngle);
		//m_SkyboxSphere->RenderModel();
		RenderCube::Render();
		// Read back the SSBO data
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

		// Map the buffer to read the data into CPU memory
		GLfloat* ptr = (GLfloat*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, 3 * sizeof(float), GL_MAP_READ_BIT);

		if (ptr) {
			// Read the vec3 result (3 floats)
			specific.finalSunlightColor = glm::vec3(ptr[0], ptr[1], ptr[2]);
			// Unmap the buffer
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		}

		// Unbind the buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		glDepthFunc(GL_LESS);
	}
}