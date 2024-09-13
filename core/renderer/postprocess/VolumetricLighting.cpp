#include "VolumetricLighting.h"
#include <utils/ServiceLocator.h>

namespace core
{
	void VolumetricLighting::Initialize()
	{
		auto winSpeci = utils::ServiceLocator::GetWindow()->GetWinSpecification();
		glGenTextures(1, &m_LightVolumeTexture);
		glBindTexture(GL_TEXTURE_3D, m_LightVolumeTexture);

		// Define the texture dimensions and format
		int depth = 32;
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, winSpeci.width, winSpeci.height, depth, 0, GL_RGBA, GL_FLOAT, NULL);

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


		std::vector<float> lightVolumeData(winSpeci.width * winSpeci.height * depth * 4, 1.0f); // Initialize with 1.0f for RGBA

		glBindTexture(GL_TEXTURE_3D, m_LightVolumeTexture);
		glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, winSpeci.width, winSpeci.height, depth, GL_RGBA, GL_FLOAT, lightVolumeData.data());

	}
	void VolumetricLighting::Dispatch()
	{
		auto winSpeci = utils::ServiceLocator::GetWindow()->GetWinSpecification();
		int depth = 32;
		glBindImageTexture(0, m_LightVolumeTexture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA32F);

		GLuint workGroupSizeX = 8; // Adjust these sizes
		GLuint workGroupSizeY = 8;
		GLuint workGroupSizeZ = 8;

		GLuint numGroupsX = (winSpeci.width + workGroupSizeX - 1) / workGroupSizeX;
		GLuint numGroupsY = (winSpeci.height + workGroupSizeY - 1) / workGroupSizeY;
		GLuint numGroupsZ = (depth + workGroupSizeZ - 1) / workGroupSizeZ;

		glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}
}