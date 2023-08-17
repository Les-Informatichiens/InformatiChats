#pragma once


#include "glm/glm.hpp"

namespace PxlUI 
{

    class BatchRenderer
    {
    public:
        static void init();
        static void shutdown();

        static void beginBatch();
        static void endBatch();
        static void flush();
        
        /* shape drawings functions */
        static void drawTriangle(const glm::vec3& iP1, const glm::vec3& iP2, const glm::vec3& iP3, const glm::vec4& iColor);
        static void drawScreenTex(uint32_t iTextureId);

        static void drawScreenTexSmaller(uint32_t iTextureId);

        struct Stats
        {
            size_t mDrawCount = 0;
        };

        static const Stats& getStats();
        static void resetStats();

    //    Renderer();
    //    ~Renderer();

    //    void resize(uint32_t iWindowWidth, uint32_t iWindowHeight);

    //private:

    //    uint32_t mWidth;
    //    uint32_t mHeight;


    //    bool mResized;
    //    const float mResolutionMultiplier;
    //    
    //    BufferData<uint32_t> mPositionArray;
    //    BufferData<uint8_t> mColorArray;
    //    BufferData<uint16_t> mZLevelArray;

    };

}

