#pragma once
#include <pch/pch.h>
#include "core/glcore/Shader.h"
#include "core/renderer/RenderQuad.h"

namespace core
{
	class IBL {
	public:
		IBL();
		~IBL();

		uint32_t GetBrdfLUTTexture() const { return m_BrdfLUTTexture; }
		uint32_t GetCubemapTexture() const { return m_CubemapTexture; }
		uint32_t GetEquiRectangleTexture() const { return m_EquiRectangleTexture; }
		uint32_t GetIrradianceTexture() const { return m_IrradianceMap; }
		uint32_t GetPrefilterTexture() const { return m_PrefilterMap; }

		void RenderAtmosphereToCubemapFace(int faceIndex);
		void RenderCubemapToEquiRectangle();
		void RenderIrradianceCubemap();
		void RenderPrefilterCubemap();
		void BindCubemapFramebuffer();
		void UnbindFramebuffer();

		glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10000.0f);
		std::vector<glm::mat4> views = {
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
		};

	private:
		void Initialize();

		// BRDF LUT
		void InitBrdf();
		void CreateBrdfLUTTexture();
		void CreateBrdfFramebuffer();
		void RenderBrdfLUT();

		// Cubemap
		void InitCubemap();
		void CreateCubemapTexture();
		void CreateCubemapFramebuffer();

		// Equirectangular Conversion
		void InitEquiRectangle();
		void CreateEquiRectangleTexture();
		void CreateEquiRectangleFramebuffer();

		void ConvertHDRToCubemap();

		// Irradiance Map
		void InitIrradianceCubemap();

		// Prefilter Map
		void InitPrefilterMap();

		// Helper Methods
		void SetFramebufferAndViewport(uint32_t framebuffer, uint32_t width, uint32_t height);



		uint32_t m_BrdfLUTTexture;
		uint32_t m_BrdfResolution;
		uint32_t m_BrdfFramebuffer;
		Shader m_BrdfShader;

		uint32_t m_HdrTexture;


		uint32_t m_CubemapTexture;
		uint32_t m_CubemapSize;
		uint32_t m_CubemapFramebuffer;

		uint32_t m_EquiRectangleTexture;
		uint32_t m_EquiRectangleSize;
		uint32_t m_EquiRectangleFramebuffer;
		Shader m_EquiRectangleShader;


		uint32_t m_IrradianceMap;
		uint32_t m_IrradianceFramebuffer;
		uint32_t m_IrradianceRenderBuffer;
		Shader m_IrradianceShader;

		uint32_t m_PrefilterMap;
		Shader m_PrefilterShader;

		RenderQuad m_RenderQuad;
	};

}