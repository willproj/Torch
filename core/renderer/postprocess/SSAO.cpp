#include "SSAO.h"
#include <utils/ServiceLocator.h>
#include "core/renderer/ShaderManager.h"
#include "core/renderer/RenderQuad.h"

namespace core
{
	SSAO::SSAO()
	{
        auto& ssaoShader = ShaderManager::GetInstance()->GetSSAOShaderRef();
        auto& ssaoBlurShader = ShaderManager::GetInstance()->GetSSAOBlurShaderRef();
        auto& winSpec = utils::ServiceLocator::GetWindow()->GetWinSpecification();
        
        ssaoShader.use();
        ssaoShader.setInt("gPosition", 0);
        ssaoShader.setInt("gNormal", 1);
        ssaoShader.setInt("texNoise", 2);
        ssaoBlurShader.use();
        ssaoBlurShader.setInt("ssaoInput", 0);
        
        this->GenerateNoiseTexture();
	}

	void SSAO::Initialize(uint32_t width, uint32_t height)
	{
        auto& speci = std::get<SSAOSpecification>(m_Specification);
        glGenFramebuffers(1, &speci.ssaoFramebuffer);
        glGenFramebuffers(1, &speci.ssaoBlurFramebuffer);

        glBindFramebuffer(GL_FRAMEBUFFER, speci.ssaoFramebuffer);
        // SSAO color buffer
        glGenTextures(1, &speci.ssaoColorTexture);
        glBindTexture(GL_TEXTURE_2D, speci.ssaoColorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, speci.ssaoColorTexture, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "SSAO Framebuffer not complete!" << std::endl;
        // and blur stage
        glBindFramebuffer(GL_FRAMEBUFFER, speci.ssaoBlurFramebuffer);
        glGenTextures(1, &speci.ssaoColorBlurTexture);
        glBindTexture(GL_TEXTURE_2D, speci.ssaoColorBlurTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, speci.ssaoColorBlurTexture, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        auto& ssaoShader = ShaderManager::GetInstance()->GetSSAOShaderRef();
        
        ssaoShader.setInt("u_ScreenWidth", width);
        ssaoShader.setInt("u_ScreenHeight", height);

	}

    SSAO::~SSAO()
    {
        auto& speci = std::get<SSAOSpecification>(m_Specification);

        if (speci.ssaoFramebuffer)
        {
            glDeleteFramebuffers(1, &speci.ssaoFramebuffer);
        }

        if (speci.ssaoBlurFramebuffer)
        {
            glDeleteFramebuffers(1, &speci.ssaoBlurFramebuffer);
        }

        if (speci.ssaoColorTexture)
        {
            glDeleteTextures(1, &speci.ssaoColorTexture);
        }

        if (speci.ssaoColorBlurTexture)
        {
            glDeleteTextures(1, &speci.ssaoColorBlurTexture);
        }
    }

    void SSAO::BeginRender()
    {
        auto& speci = std::get<SSAOSpecification>(m_Specification);
        auto& ssaoShader = ShaderManager::GetInstance()->GetSSAOShaderRef();
        // 2. generate SSAO texture
        // ------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, speci.ssaoFramebuffer);
        glClear(GL_COLOR_BUFFER_BIT);
        ssaoShader.use();
        // Send kernel + rotation 
        for (unsigned int i = 0; i < 64; ++i)
            ssaoShader.setVec3("samples[" + std::to_string(i) + "]", speci.ssaoKernel[i]);
        ssaoShader.setMat4("projection", speci.camera->GetProjection());
    }

    void SSAO::EndRender()
    {

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        auto& speci = std::get<SSAOSpecification>(m_Specification);

        // 3. blur SSAO texture to remove noise
       // ------------------------------------
        auto& ssaoBlurShader = ShaderManager::GetInstance()->GetSSAOBlurShaderRef();
        glBindFramebuffer(GL_FRAMEBUFFER, speci.ssaoBlurFramebuffer);
        glClear(GL_COLOR_BUFFER_BIT);
        ssaoBlurShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, speci.ssaoColorTexture);
        RenderQuad::Render();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void SSAO::OnUpdate(uint32_t width, uint32_t height)
    {
        auto& speci = std::get<SSAOSpecification>(m_Specification);
        if (speci.ssaoFramebuffer)
        {
            glDeleteFramebuffers(1, &speci.ssaoFramebuffer);
        }

        if (speci.ssaoBlurFramebuffer)
        {
            glDeleteFramebuffers(1, &speci.ssaoBlurFramebuffer);
        }

        if (speci.ssaoColorTexture)
        {
            glDeleteTextures(1, &speci.ssaoColorTexture);
        }

        if (speci.ssaoColorBlurTexture)
        {
            glDeleteTextures(1, &speci.ssaoColorBlurTexture);
        }

        this->Initialize(width, height);
    }
    void SSAO::GenerateNoiseTexture()
    {
        auto& speci = std::get<SSAOSpecification>(m_Specification);
        std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
        std::default_random_engine generator;
        for (unsigned int i = 0; i < 64; ++i)
        {
            glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
            sample = glm::normalize(sample);
            sample *= randomFloats(generator);
            float scale = float(i) / 64.0f;

            // scale samples s.t. they're more aligned to center of kernel
            scale = Lerp(0.1f, 1.0f, scale * scale);
            sample *= scale;
            speci.ssaoKernel.push_back(sample);
        }

        // generate noise texture
        // ----------------------
        std::vector<glm::vec3> ssaoNoise;
        for (unsigned int i = 0; i < 16; i++)
        {
            glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
            ssaoNoise.push_back(noise);
        }
        glGenTextures(1, &speci.noiseTexture);
        glBindTexture(GL_TEXTURE_2D, speci.noiseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
}