#include "BatchRenderer.h"

#include "GL/glew.h"

#include "ShaderProgram.h"

#include <array>

namespace PxlUI {

    static const float skDefaultResolutionMult = 0.5f;
    static const size_t skMaxQuadCount = 1000;
    static const size_t skMaxVertexCount = 3 * 1024;
    static const size_t skMaxTextures = 32;

    struct Vertex
    {
        glm::vec3 mPosition;
        glm::vec4 mColor;
        glm::vec2 mTexCoords;
        float mTexIndex;
    };

    struct RendererData
    {

        GLuint mVertexArray = 0;
        GLuint mVertexBuffer = 0;

        GLuint mWhiteTexture = 0;
        uint32_t mWhiteTextureSlot = 0;

        Vertex *mBufferData = nullptr;
        Vertex *mBufferDataPtr = nullptr;

        size_t mVertexCount = 0;

        std::array<uint32_t, skMaxTextures> mTextureSlots;
        uint32_t mTextureSlotIndex = 1;

        BatchRenderer::Stats renderStats;
    };

    static RendererData sData;

    void BatchRenderer::init()
    {

        sData.mBufferData = new Vertex[skMaxVertexCount];

        glCreateVertexArrays(1, &sData.mVertexArray);
        glBindVertexArray(sData.mVertexArray);

        glCreateBuffers(1, &sData.mVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, sData.mVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, skMaxVertexCount * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexArrayAttrib(sData.mVertexArray, 0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, mPosition));

        glEnableVertexArrayAttrib(sData.mVertexArray, 1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, mColor));

        glEnableVertexArrayAttrib(sData.mVertexArray, 2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, mTexCoords));

        glEnableVertexArrayAttrib(sData.mVertexArray, 3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, mTexIndex));

        glCreateTextures(GL_TEXTURE_2D, 1, &sData.mWhiteTexture);
        glBindTexture(GL_TEXTURE_2D, sData.mWhiteTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        uint32_t wColor = 0xffffffff;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &wColor);

        sData.mTextureSlots[0] = sData.mWhiteTexture;
        for (size_t i = 1; i < skMaxTextures; i++)
        {
            sData.mTextureSlots[i] = 0;
        }
    }

    void BatchRenderer::shutdown()
    {
        glDeleteVertexArrays(1, &sData.mVertexArray);
        glDeleteBuffers(1, &sData.mVertexBuffer);
        glDeleteTextures(1, &sData.mWhiteTexture);

        delete[] sData.mBufferData;
    }

    void BatchRenderer::beginBatch()
    {
        sData.mBufferDataPtr = sData.mBufferData;
    }

    void BatchRenderer::endBatch()
    {
        GLsizeiptr wSize = (uint8_t *) sData.mBufferDataPtr - (uint8_t *) sData.mBufferData;
        glBindBuffer(GL_ARRAY_BUFFER, sData.mVertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, wSize, sData.mBufferData);
    }

    void BatchRenderer::flush()
    {
        for (uint32_t i = 0; i < sData.mTextureSlotIndex; i++)
        {
            glBindTextureUnit(i, sData.mTextureSlots[i]);
        }
        glBindVertexArray(sData.mVertexArray);
        glDrawArrays(GL_TRIANGLES, 0, sData.mVertexCount);

        sData.mVertexCount = 0;
        sData.mTextureSlotIndex = 1;

        sData.renderStats.mDrawCount++;
    }

    void BatchRenderer::drawTriangle(const glm::vec3 &iP1, const glm::vec3 &iP2, const glm::vec3 &iP3,
                                     const glm::vec4 &iColor)
    {
        if (sData.mVertexCount >= skMaxVertexCount)
        {
            endBatch();
            flush();
            beginBatch();
        }

        float wTexIndex = 0.f;

        sData.mBufferDataPtr->mPosition = {iP1.x, iP1.y, iP1.z};
        sData.mBufferDataPtr->mColor = iColor;
        sData.mBufferDataPtr->mTexCoords = {0.f, 0.f};
        sData.mBufferDataPtr->mTexIndex = wTexIndex;
        sData.mBufferDataPtr++;

        sData.mBufferDataPtr->mPosition = {iP2.x, iP2.y, iP2.z};
        sData.mBufferDataPtr->mColor = iColor;
        sData.mBufferDataPtr->mTexCoords = {0.f, 0.f};
        sData.mBufferDataPtr->mTexIndex = wTexIndex;
        sData.mBufferDataPtr++;

        sData.mBufferDataPtr->mPosition = {iP3.x, iP3.y, iP3.z};
        sData.mBufferDataPtr->mColor = iColor;
        sData.mBufferDataPtr->mTexCoords = {0.f, 0.f};
        sData.mBufferDataPtr->mTexIndex = wTexIndex;
        sData.mBufferDataPtr++;

        sData.mVertexCount += 3;
    }

    void BatchRenderer::drawScreenTex(uint32_t iTextureId)
    {
        if (sData.mVertexCount >= skMaxVertexCount)
        {
            endBatch();
            flush();
            beginBatch();
        }

        constexpr glm::vec4 wColor = {1.f, 1.f, 1.f, 1.f};

        float wTexIndex = 0.f;
        for (uint32_t i = 1; i < sData.mTextureSlotIndex; i++)
        {
            if (sData.mTextureSlots[i] == iTextureId)
            {
                wTexIndex = (float) i;
                break;
            }
        }

        if (wTexIndex == 0.f)
        {
            wTexIndex = (float) sData.mTextureSlotIndex;
            sData.mTextureSlots[sData.mTextureSlotIndex] = iTextureId;
            sData.mTextureSlotIndex++;
        }

        sData.mBufferDataPtr->mPosition = {-1.f, -1.f, 0.f};
        sData.mBufferDataPtr->mColor = wColor;
        sData.mBufferDataPtr->mTexCoords = {0.f, 0.f};
        sData.mBufferDataPtr->mTexIndex = wTexIndex;
        sData.mBufferDataPtr++;

        sData.mBufferDataPtr->mPosition = {1.f, -1.f, 0.f};
        sData.mBufferDataPtr->mColor = wColor;
        sData.mBufferDataPtr->mTexCoords = {1.f, 0.f};
        sData.mBufferDataPtr->mTexIndex = wTexIndex;
        sData.mBufferDataPtr++;

        sData.mBufferDataPtr->mPosition = {-1.f, 1.f, 0.f};
        sData.mBufferDataPtr->mColor = wColor;
        sData.mBufferDataPtr->mTexCoords = {0.f, 1.f};
        sData.mBufferDataPtr->mTexIndex = wTexIndex;
        sData.mBufferDataPtr++;

        sData.mBufferDataPtr->mPosition = {-1.f, 1.f, 0.f};
        sData.mBufferDataPtr->mColor = wColor;
        sData.mBufferDataPtr->mTexCoords = {0.f, 1.f};
        sData.mBufferDataPtr->mTexIndex = wTexIndex;
        sData.mBufferDataPtr++;

        sData.mBufferDataPtr->mPosition = {1.f, -1.f, 0.f};
        sData.mBufferDataPtr->mColor = wColor;
        sData.mBufferDataPtr->mTexCoords = {1.f, 0.f};
        sData.mBufferDataPtr->mTexIndex = wTexIndex;
        sData.mBufferDataPtr++;

        sData.mBufferDataPtr->mPosition = {1.f, 1.f, 0.f};
        sData.mBufferDataPtr->mColor = wColor;
        sData.mBufferDataPtr->mTexCoords = {1.f, 1.f};
        sData.mBufferDataPtr->mTexIndex = wTexIndex;
        sData.mBufferDataPtr++;

        sData.mVertexCount += 6;
    }

    void BatchRenderer::drawScreenTexSmaller(uint32_t iTextureId)
    {
        if (sData.mVertexCount >= skMaxVertexCount)
        {
            endBatch();
            flush();
            beginBatch();
        }

        constexpr glm::vec4 wColor = {1.f, 0.f, 0.f, 0.1f};

        float wTexIndex = 0.f;
        for (uint32_t i = 1; i < sData.mTextureSlotIndex; i++)
        {
            if (sData.mTextureSlots[i] == iTextureId)
            {
                wTexIndex = (float) i;
                break;
            }
        }

        if (wTexIndex == 0.f)
        {
            wTexIndex = (float) sData.mTextureSlotIndex;
            sData.mTextureSlots[sData.mTextureSlotIndex] = iTextureId;
            sData.mTextureSlotIndex++;
        }

        sData.mBufferDataPtr->mPosition = {-0.5f, -0.5f, 0.f};
        sData.mBufferDataPtr->mColor = wColor;
        sData.mBufferDataPtr->mTexCoords = {0.f, 0.f};
        sData.mBufferDataPtr->mTexIndex = wTexIndex;
        sData.mBufferDataPtr++;

        sData.mBufferDataPtr->mPosition = {0.5f, -0.5f, 0.f};
        sData.mBufferDataPtr->mColor = wColor;
        sData.mBufferDataPtr->mTexCoords = {1.f, 0.f};
        sData.mBufferDataPtr->mTexIndex = wTexIndex;
        sData.mBufferDataPtr++;

        sData.mBufferDataPtr->mPosition = {-0.5f, 0.5f, 0.f};
        sData.mBufferDataPtr->mColor = wColor;
        sData.mBufferDataPtr->mTexCoords = {0.f, 1.f};
        sData.mBufferDataPtr->mTexIndex = wTexIndex;
        sData.mBufferDataPtr++;

        sData.mBufferDataPtr->mPosition = {-0.5f, 0.5f, 0.f};
        sData.mBufferDataPtr->mColor = wColor;
        sData.mBufferDataPtr->mTexCoords = {0.f, 1.f};
        sData.mBufferDataPtr->mTexIndex = wTexIndex;
        sData.mBufferDataPtr++;

        sData.mBufferDataPtr->mPosition = {0.5f, -0.5f, 0.f};
        sData.mBufferDataPtr->mColor = wColor;
        sData.mBufferDataPtr->mTexCoords = {1.f, 0.f};
        sData.mBufferDataPtr->mTexIndex = wTexIndex;
        sData.mBufferDataPtr++;

        sData.mBufferDataPtr->mPosition = {0.5f, 0.5f, 0.f};
        sData.mBufferDataPtr->mColor = wColor;
        sData.mBufferDataPtr->mTexCoords = {1.f, 1.f};
        sData.mBufferDataPtr->mTexIndex = wTexIndex;
        sData.mBufferDataPtr++;

        sData.mVertexCount += 6;
    }

    const BatchRenderer::Stats &BatchRenderer::getStats()
    {
        return sData.renderStats;
    }

    void BatchRenderer::resetStats()
    {
        sData.renderStats = {};
    }

}// namespace PxlUI
