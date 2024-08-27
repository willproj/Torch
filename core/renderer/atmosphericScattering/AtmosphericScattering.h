#pragma once
#include <pch/pch.h>
#include "core/glcore/Shader.h"

namespace core
{
	class AtmosphericScattering
	{
	public:
		AtmosphericScattering()
		{
			m_AtmosphereProgram  = std::make_shared<Shader>("assets/shader/shader.vert", "assets/shader/frag.frag");
			Initialize();
		}
		~AtmosphericScattering()
		{

		}

		void Initialize()
		{
			m_ViewSamples = m_DefLightSamples;
			m_LightSamples = m_LightSamples;

			m_SunDir = m_DefSunDir;
			SetSunAngle(m_DefSunAngle);
			m_I_Sun = m_E_I_Sun;
			SetEarthRadius(m_E_R_E);
			SetAtmosRadius(m_E_R_A);

			m_Beta_R = m_E_Beta_R;
			m_Beta_M = m_E_Beta_M;
			m_H_R = m_E_H_R;
			m_H_M = m_E_H_M;
			m_G = m_E_G;
		}

		void SetSunDefaults()
		{
			m_AnimateSun = false;
			m_SunDir = m_DefSunDir;
			SetSunAngle(m_DefSunAngle);
			m_I_Sun = m_E_I_Sun;
		}

		void SetRayleighDefaults()
		{
			m_Beta_R = m_E_Beta_R;
			m_H_R = m_E_H_R;
		}

		void SetMieDefaults()
		{
			m_Beta_M = m_E_Beta_M;
			m_H_M = m_E_H_M;
			m_G = m_E_G;
		}

		void SetSizeDefaults()
		{
			SetEarthRadius(m_E_R_E);
			SetAtmosRadius(m_E_R_A);
			m_RenderEarth = false;
		}

		void SetProjectView(const glm::mat4& proj, const glm::mat4& view)
		{
			m_Proj = proj;
			m_View = view;
		}

		void SetViewPos(const glm::vec3& cameraPos)
		{
			m_ViewPos = cameraPos;
		}

		void SetViewSamples(int samples)
		{
			m_ViewSamples = samples;
		}

		void SetLightSamples(int samples)
		{
			m_LightSamples = samples;
		}

		void SetToneMapping(bool b)
		{
			m_ToneMapping = b;
		}

		void SetAnimateSun(bool b)
		{
			m_AnimateSun = b;
		}

		void SetSunAngle(float angle)
		{
			m_SunAngle = angle;
			m_SunDir.y = glm::sin(m_SunAngle);
			m_SunDir.z = -glm::cos(m_SunAngle);
		}

		void SetSunDir(const glm::vec3& dir)
		{
			m_SunDir = dir;
		}

		void SetSunIntensity(float I)
		{
			m_I_Sun = I;
		}

		void SetEarthRadius(float R)
		{
			m_R_E = R;
			ModelEarth();
		}

		void SetAtmosRadius(float R)
		{
			m_R_A = R;
			ModelAtoms();
		}

		void SetRayleighScattering(const glm::vec3& beta_s)
		{
			m_Beta_R = beta_s;
		}

		void SetRayleighScaleHeight(float H)
		{
			m_H_R = H;
		}

		void SetMieScattering(float beta_s)
		{
			m_Beta_M = beta_s;
		}

		void SetScaleHeight(float H)
		{
			m_H_M = H;
		}

		void SetMieScatteringDir(float d)
		{
			m_G = d;
		}

		void SetRenderEarth(bool b)
		{
			m_RenderEarth = b;
		}




	private:
		//rendering
		std::shared_ptr<Shader> m_AtmosphereProgram;
		std::shared_ptr<Shader> m_DrawMeshProgram;


		glm::mat4 m_ModelAtoms; //model matrix for atmosphere
		glm::mat4 m_ModelEarth; //model matrix for planet

		glm::mat4 m_Proj;
		glm::mat4 m_View;

		void ModelAtoms() 
		{
			m_ModelAtoms = glm::scale(glm::mat4(1.0f), glm::vec3(m_R_A, m_R_A, m_R_A));
		}

		void ModelEarth()
		{
			m_ModelEarth = glm::scale(glm::mat4(1.0f), glm::vec3(m_R_A, m_R_E, m_R_E));
		}

		glm::vec3 m_ViewPos; //position of viewer, camera
		int m_ViewSamples;  //number of samples along the view primary ray
		int m_LightSamples; //number of samples along the lights

		//gui 
		bool m_ToneMapping = true;
		bool m_AnimateSun = false;
		float m_SunAngle;
		bool m_RenderEarth = false;


		//Atmospheric constants
		glm::vec3 m_SunDir;
		float m_I_Sun; //intensity of sun
		float m_R_E; //radius of earth
		float m_R_A; //radius of the atmosphere

		glm::vec3 m_Beta_R; //rayleigh scattering coefficient
		float m_H_R; // rayleigh scale height

		float m_Beta_M; //mie scattering coefficient
		float m_H_M; //mie scale height
		float m_G; //mie scattering direction


		// Defaults
		static inline const int m_DefViewSamples = 16;
		static inline const int m_DefLightSamples = 8;
		static inline const float m_DefSunAngle = glm::radians(1.1f);

		static inline const glm::vec3 m_DefSunDir = glm::vec3(0, 1, 0);

		//Earth 
		static inline const float m_E_I_Sun = 20.0f;
		static inline const float m_E_R_E = 6360.0f;
		static inline const float m_E_R_A = 6420.0f;
		static inline const glm::vec3 m_E_Beta_R = glm::vec3(5.8e-3f, 13.5e-3f, 33.1e-3f);

		static inline const float m_E_Beta_M = 21e-3f;
		static inline const float m_E_H_R = 7.994f;
		static inline const float m_E_H_M = 1.200f;
		static inline const float m_E_G = 0.888f;

		//conversions
		static inline const float m_M_2_KM = 0.001f;
		static inline const float m_KM_2_M = 1000.0f;
	};
}