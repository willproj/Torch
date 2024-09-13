#pragma once
#include <pch/pch.h>
#include "core/glcore/Shader.h"

namespace core
{
	class VolumetricLighting
	{
	public:
		VolumetricLighting() = default;
		~VolumetricLighting() = default;

		void Initialize();

		uint32_t GetLightVolumeTexture() const { return m_LightVolumeTexture; }

		void Dispatch();
	private:
		uint32_t m_LightVolumeTexture;
	};
}