//
// Created by Jonathan Richard on 2023-08-07.
//

#ifndef INFORMATICHATS_BATCHRENDERER_H
#define INFORMATICHATS_BATCHRENDERER_H


#include <glm/glm.hpp>

#include "BatchRenderer.h"

    class BatchRenderer
    {
    public:
        static void init();
        static void setup();
        static void shutdown();

        static void beginBatch();
        static void endBatch();
        static void flush();

        /* shape drawings functions */
        static void drawTriangle(const glm::vec2& iP1, const glm::vec2& iP2, const glm::vec2& iP3, uint32_t iColor);
        static void drawScreenTex(uint32_t iTextureId);

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

#endif //INFORMATICHATS_BATCHRENDERER_H
