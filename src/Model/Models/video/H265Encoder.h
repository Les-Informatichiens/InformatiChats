//
// Created by thierrycd on 2024-01-09.
//

#pragma once

#include "IEncoder.h"

#include <iostream>
#include <memory>

extern "C" {
#include "libavcodec/avcodec.h"
#include <libavdevice/avdevice.h>
}


class H265Encoder : public IEncoder
{
public:
    H265Encoder()
    {
        codec = avcodec_find_encoder(AV_CODEC_ID_H265);
        if (!codec)
        {
            std::cerr << "Codec not found" << std::endl;
            return;
        }

        encoderContext = avcodec_alloc_context3(codec);
        pkt = av_packet_alloc();

        /* put sample parameters */
        encoderContext->bit_rate = 400000;
        /* resolution must be a multiple of two */
        encoderContext->width = 352;
        encoderContext->height = 288;
        /* frames per second */
        encoderContext->time_base = AVRational{1, 25};
        encoderContext->framerate = AVRational{25, 1};

        encoderContext->gop_size = 10;
        encoderContext->max_b_frames = 1;
        encoderContext->pix_fmt = AV_PIX_FMT_YUV420P;

        if (const int ret = avcodec_open2(encoderContext, codec, nullptr); ret < 0)
        {
            std::cerr << "Could not open codec" << std::endl;
            return;
        }

        frame = av_frame_alloc();
        if (!frame)
        {
            std::cerr << "Could not allocate video frame" << std::endl;
            return;
        }
        frame->format = encoderContext->pix_fmt;
        frame->width = encoderContext->width;
        frame->height = encoderContext->height;


        if (const int ret = av_frame_get_buffer(frame, 0); ret < 0)
        {
            fprintf(stderr, "Could not allocate the video frame data\n");
            return;
        }
    }

    ~H265Encoder() override
    {
        avcodec_free_context(&encoderContext);
        av_frame_free(&frame);
        av_packet_free(&pkt);
    }

    void encode() override {}
    void decode() override {}

private:
    const AVCodec* codec;
    AVDeviceInfo deviceInfo{};
    AVCodecContext* encoderContext{};
    AVFrame* frame;
    AVPacket* pkt;
};
