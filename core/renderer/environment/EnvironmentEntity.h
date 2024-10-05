#pragma once
#include <pch/pch.h>
#include "core/renderer/camera/EditorCamera.h"
namespace core
{
	enum class EnvironmentEntityType
	{
		None,
		Atmosphere,
		VolmetricFog,
		CascadeShadowMap,
		SSAO
	};


	struct AtmosphericScatteringSpecification
	{

		//sun position = sun direction skybox shader naming issue
		glm::vec3 sunPosition;

		glm::vec3 sunColor;

		//1.0f default
		float sunIntensity;

		// [0.0001, 0.01]
		float rayTMin;

		//[6350000.0, 6400000.0]
		float earthRadius;
		//[90000.0, 120000.0]
		float atmosphereHeight;

		//[6000.0, 10000.0]
		float rayleighHeight;

		//[1e-7, 5e-5]
		glm::vec3 rayleighScatteringCoef;

		//[1000.0, 1500.0]
		float mieHeight;

		//[1e-7, 5e-5]
		glm::vec3 mieScatteringCoef;

		//[1e-6, 1e-4]
		glm::vec3 ozoneAbsorptionCoef;

		//[10- 200]
		uint32_t sampleCount;

		//[0.1 * PI / 180.0, 5.0 * PI / 180.0]
		float sunAngle;

		//[0.1, 10.0]
		float exposure;

		//[1.0,3.0]
		float gamma;

		//[0.0, 1.0] This controls the asymmetry factor g in the phase function for Mie scattering
		float henyeyGreensteinCoef;

		//
		glm::vec3 finalSunlightColor;
	};

	struct CascadeShadowMapSpecification
	{
		uint32_t shadowMapFramebuffer;
		uint32_t depthMapResolution = 4096;
		uint32_t shadowMapTexture;

		std::vector<float> shadowCascadeLevels{ 10.0f, 20.0f, 80.0f, 640.0f, 5000.0f };
	};

	using SpecificationVariant = std::variant
	<
		AtmosphericScatteringSpecification, 
		CascadeShadowMapSpecification
	>;
	
	class EnvironmentEntity
	{
	public:
		EnvironmentEntity() = default;
		virtual ~EnvironmentEntity() = default;

		virtual void Render() = 0;

		virtual void BeginRender() {};
		virtual void EndRender() {};

		virtual void Render(const glm::mat4& view, const glm::mat4& projection) {};
		virtual void SetShader() {};
		virtual void Initialize() {};
		virtual void Initialize(uint32_t width, uint32_t height) {};
		virtual utils::Ref<SpecificationVariant> GetSpecification() = 0;
		virtual EnvironmentEntityType GetType() const { return m_Type; }
		virtual void OnUpdate() {};
		virtual void OnUpdate(uint32_t width, uint32_t height) {};
		virtual bool IsRunning() { return false;  };
		virtual void SetIsRunning(bool isRunning) { };
	protected:
		EnvironmentEntityType m_Type = EnvironmentEntityType::None;
		std::shared_ptr<EditorCamera> m_CurrentCamera= nullptr;
	};
}