//
// Created by thierrycd on 2024-01-09.
//

#pragma once

#include "IEncoder.h"

#include <iostream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
}


class H265Encoder : public IEncoder
{
public:
    H265Encoder()
    {
        FILE* outfile = fopen("test.h264", "wb");

        avdevice_register_all();

        // Open desktop video source (gdigrab)
        AVFormatContext* input_ctx = avformat_alloc_context();
        const AVInputFormat* ifmt = av_find_input_format("gdigrab");
        AVDictionary* options = NULL;

        av_dict_set(&options, "framerate", "30", 0);
        av_dict_set(&options, "offset_x", "20", 0);
        av_dict_set(&options, "offset_y", "40", 0);
        av_dict_set(&options, "video_size", "640x480", 0);
        av_dict_set(&options, "video_codec_id", "AV_CODEC_ID_HVEC", 0);
        if (avformat_open_input(&input_ctx, "desktop", ifmt, &options) < 0)
        {
            std::cerr << "Error opening desktop video source" << std::endl;
            return;
        }

        // Find stream information
        if (avformat_find_stream_info(input_ctx, NULL) < 0)
        {
            std::cerr << "Error finding stream information" << std::endl;
            avformat_close_input(&input_ctx);
            return;
        }

        // Find the video stream
        const AVCodec* input_codec = NULL;
        int video_stream_index = av_find_best_stream(input_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &input_codec, 0);

        if (video_stream_index < 0)
        {
            std::cerr << "Error finding video stream" << std::endl;
            avformat_close_input(&input_ctx);
            return;
        }

        codec_ctx = avcodec_alloc_context3(input_codec);

        // Find the H.264 video encoder
        this->encoder = avcodec_find_encoder(AV_CODEC_ID_H265);
        if (!encoder)
        {
            fprintf(stderr, "Error finding H.265 encoder\n");
            avformat_close_input(&input_ctx);
            return;
        }

        // Allocate and open the encoder context
        encoder_ctx = avcodec_alloc_context3(encoder);

        encoder_ctx->width = 640; // Set your screen width
        encoder_ctx->height = 480;// Set your screen height
        encoder_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
        encoder_ctx->time_base = AVRational{1, 30};// Set the frame rate
        encoder_ctx->framerate = AVRational{30, 1};// Set the frame rate
        // avcodec_parameters_to_context(encoder_ctx, input_ctx->streams[video_stream_index]->codecpar);

        if (avcodec_open2(encoder_ctx, encoder, &options) < 0)
        {
            std::cerr << "Error opening the encoder" << std::endl;
            avcodec_free_context(&encoder_ctx);
            avformat_close_input(&input_ctx);
            return;
        }

        // Allocate an AVFrame for storing frames
        AVFrame* frame = av_frame_alloc();


        // Allocate an AVPacket for storing encoded packets
        pkt = av_packet_alloc();

        // Main loop to read frames, encode, and send
        int frame_count = 0;
        while (av_read_frame(input_ctx, pkt) >= 0)
        {
            if (pkt->stream_index == video_stream_index)
            {
                // Decode the frame
                if (avcodec_receive_frame(codec_ctx, frame) == 0)
                {
                    // Encode the frame
                    if (avcodec_send_frame(encoder_ctx, frame) == 0)
                    {
                        while (avcodec_receive_packet(encoder_ctx, pkt) == 0)
                        {
                            fwrite(pkt->data, 1, pkt->size, outfile);
                            // Send the packet (you need to implement this part)
                            // Example: send_packet_to_network(packet);
                            av_packet_unref(pkt);
                            frame_count++;
                        }
                    }
                }
            }

            // Free the packet
            av_packet_unref(pkt);
        }
    }

    ~H265Encoder() override
    {
        av_frame_free(&frame);
        av_packet_free(&pkt);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&input_ctx);
    }

    std::vector<std::byte> encode(const std::vector<std::byte>& frame) override { return {}; };
    std::vector<std::byte> decode(const std::vector<std::byte>& encodedFrame) override { return {}; };

private:
    void encode(AVCodecContext* enc_ctx, AVFrame* frame, AVPacket* pkt,
                FILE* outfile){};

    const AVCodec* encoder;
    AVDeviceInfo* deviceInfo;
    AVCodecContext *codec_ctx, *encoder_ctx;
    AVFormatContext *input_ctx, *output_ctx;
    AVFrame* frame;
    AVPacket* pkt;
    FILE* outfile;
};
