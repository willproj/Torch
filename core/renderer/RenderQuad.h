#pragma once
#include <pch/pch.h>

namespace core
{
    class RenderQuad
    {
    private:
        inline static unsigned int quadVAO = 0;
        inline static unsigned int quadVBO;
    public:
        static void Render()
        {
            if (quadVAO == 0)
            {
                float quadVertices[] = {
                    // positions        // texture Coords
                    -1.0f,
                    1.0f,
                    0.0f,
                    0.0f,
                    1.0f,
                    -1.0f,
                    -1.0f,
                    0.0f,
                    0.0f,
                    0.0f,
                    1.0f,
                    1.0f,
                    0.0f,
                    1.0f,
                    1.0f,
                    1.0f,
                    -1.0f,
                    0.0f,
                    1.0f,
                    0.0f,
                };
                // setup plane VAO
                glGenVertexArrays(1, &quadVAO);
                glGenBuffers(1, &quadVBO);
                glBindVertexArray(quadVAO);
                glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
            }
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindVertexArray(0);
        }
    };
}