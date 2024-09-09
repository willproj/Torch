#include "SSAO.h"
#include <utils/ServiceLocator.h>
#include "core/renderer/ShaderManager.h"
#include "core/renderer/RenderQuad.h"

namespace core
{
	SSAO::SSAO(std::shared_ptr<EditorCamera> camera)
	{
		m_EditorCameraPtr = camera;
        auto& ssaoShader = ShaderManager::GetInstance()->GetSSAOShaderRef();
        auto& ssaoBlurShader = ShaderManager::GetInstance()->GetSSAOBlurShaderRef();
        
        ssaoShader.use();
        ssaoShader.setInt("gPosition", 0);
        ssaoShader.setInt("gNormal", 1);
        ssaoShader.setInt("texNoise", 2);
        ssaoBlurShader.use();
        ssaoBlurShader.setInt("ssaoInput", 0);
        
        GenerateNoiseTexture();
        Initialize();
	}


	void SSAO::Initialize()
	{
        auto& specific = std::get<SSAOSpecification>(m_Specification);
        auto winSpeci = utils::ServiceLocator::GetWindow()->GetWinSpecification();
        glGenFramebuffers(1, &specific.ssaoFramebuffer);  
        glGenFramebuffers(1, &specific.ssaoBlurFramebuffer);

        glBindFramebuffer(GL_FRAMEBUFFER, specific.ssaoFramebuffer);
        // SSAO color buffer
        glGenTextures(1, &specific.ssaoColorTexture);
        glBindTexture(GL_TEXTURE_2D, specific.ssaoColorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, winSpeci.width, winSpeci.height, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, specific.ssaoColorTexture, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "SSAO Framebuffer not complete!" << std::endl;
        // and blur stage
        glBindFramebuffer(GL_FRAMEBUFFER, specific.ssaoBlurFramebuffer);
        glGenTextures(1, &specific.ssaoColorBlurTexture);
        glBindTexture(GL_TEXTURE_2D, specific.ssaoColorBlurTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, winSpeci.width, winSpeci.height, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, specific.ssaoColorBlurTexture, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        auto& ssaoShader = ShaderManager::GetInstance()->GetSSAOShaderRef();
        
        ssaoShader.setInt("u_ScreenWidth", winSpeci.width);
        ssaoShader.setInt("u_ScreenHeight", winSpeci.height);

	}

	void SSAO::Render()
	{
        // 3. blur SSAO texture to remove noise
        // ------------------------------------
        auto& ssaoBlurShader = ShaderManager::GetInstance()->GetSSAOBlurShaderRef();
        auto& specific = std::get<SSAOSpecification>(m_Specification);
        glBindFramebuffer(GL_FRAMEBUFFER, specific.ssaoBlurFramebuffer);
        glClear(GL_COLOR_BUFFER_BIT);
        ssaoBlurShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, specific.ssaoColorTexture);
        RenderQuad::Render();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
	}

    void SSAO::BeginRender()
    {
        auto& specific = std::get<SSAOSpecification>(m_Specification);
        auto& ssaoShader = ShaderManager::GetInstance()->GetSSAOShaderRef();
        // 2. generate SSAO texture
        // ------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, specific.ssaoFramebuffer);
        glClear(GL_COLOR_BUFFER_BIT);
        ssaoShader.use();
        // Send kernel + rotation 
        for (unsigned int i = 0; i < 64; ++i)
            ssaoShader.setVec3("samples[" + std::to_string(i) + "]", specific.ssaoKernel[i]);
        ssaoShader.setMat4("projection", m_EditorCameraPtr->GetProjection());
    }

    void SSAO::EndRender()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void SSAO::OnUpdate()
    {
        auto& specific = std::get<SSAOSpecification>(m_Specification);
        auto winSpeci = utils::ServiceLocator::GetWindow()->GetWinSpecification();
        if (specific.ssaoFramebuffer)
        {
            glDeleteFramebuffers(1, &specific.ssaoFramebuffer);
        }

        if (specific.ssaoBlurFramebuffer)
        {
            glDeleteFramebuffers(1, &specific.ssaoBlurFramebuffer);
        }

        if (specific.ssaoColorTexture)
        {
            glDeleteTextures(1, &specific.ssaoColorTexture);
        }

        if (specific.ssaoColorBlurTexture)
        {
            glDeleteTextures(1, &specific.ssaoColorBlurTexture);
        }

        Initialize();
    }
    void SSAO::GenerateNoiseTexture()
    {
        auto& specific = std::get<SSAOSpecification>(m_Specification);

        std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
        std::default_random_engine generator;
        for (unsigned int i = 0; i < 64; ++i)
        {
            glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
            sample = glm::normalize(sample);
            sample *= randomFloats(generator);
            float scale = float(i) / 64.0f;

            // scale samples s.t. they're more aligned to center of kernel
            scale = ourLerp(0.1f, 1.0f, scale * scale);
            sample *= scale;
            specific.ssaoKernel.push_back(sample);
        }

        // generate noise texture
        // ----------------------
        std::vector<glm::vec3> ssaoNoise;
        for (unsigned int i = 0; i < 16; i++)
        {
            glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
            ssaoNoise.push_back(noise);
        }
        glGenTextures(1, &specific.noiseTexture);
        glBindTexture(GL_TEXTURE_2D, specific.noiseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
}