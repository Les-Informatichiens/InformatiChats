#include "IEncoder.h"
#include <cstdio>
#include <iostream>
#include <vector>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
}

class H265Encoder : public IEncoder
{
public:
    H265Encoder()
    {
        // Register all codecs and formats and network
        avdevice_register_all();
        avformat_network_init();

        // Initialize input context for desktop capture
        input_ctx = avformat_alloc_context();
        if (!input_ctx)
        {
            std::cerr << "Failed to allocate input context" << std::endl;
            return;
        }

        const AVInputFormat* ifmt = av_find_input_format("gdigrab");
        if (!ifmt)
        {
            std::cerr << "Failed to find input format 'gdigrab'" << std::endl;
            return;
        }

        AVDictionary* options = NULL;
        av_dict_set(&options, "framerate", "30", 0);
        av_dict_set(&options, "video_size", "1920x1080", 0);// Adjust as needed

        if (avformat_open_input(&input_ctx, "desktop", ifmt, &options) != 0)
        {
            std::cerr << "Failed to open input device" << std::endl;
            return;
        }

        // Initialize stream info
        if (avformat_find_stream_info(input_ctx, NULL) < 0)
        {
            std::cerr << "Failed to find stream information" << std::endl;
            return;
        }

        // Initialize codec and encoder contexts for H.265
        const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_HEVC);
        if (!codec)
        {
            std::cerr << "Codec not found" << std::endl;
            return;
        }

        encoder_ctx = avcodec_alloc_context3(codec);
        if (!encoder_ctx)
        {
            std::cerr << "Could not allocate video codec context" << std::endl;
            return;
        }

        AVStream* stream = input_ctx->streams[0];// Assuming the first stream is the desired one
        encoder_ctx->width = stream->codecpar->width;
        encoder_ctx->height = stream->codecpar->height;
        encoder_ctx->pix_fmt = AV_PIX_FMT_YUV420P;                                  // Adjust as needed
        encoder_ctx->time_base = stream->time_base;                                 // Use the stream's time base
        encoder_ctx->framerate = av_inv_q(stream->time_base);                       // Inverse of the time base

        if (avcodec_open2(encoder_ctx, codec, NULL) < 0)
        {
            std::cerr << "Could not open codec" << std::endl;
            return;
        }

        // Initialize decoder context for H.265
        decoder_ctx = avcodec_alloc_context3(codec);
        if (!decoder_ctx)
        {
            std::cerr << "Could not allocate video codec context" << std::endl;
            return;
        }
        decoder_ctx->width = stream->codecpar->width;
        decoder_ctx->height = stream->codecpar->height;
        decoder_ctx->pix_fmt = AV_PIX_FMT_YUV420P;           // Adjust as needed
        decoder_ctx->time_base = stream->time_base;          // Use the stream's time base
        decoder_ctx->framerate = av_inv_q(stream->time_base);// Inverse of the time base

        if (avcodec_open2(decoder_ctx, codec, NULL) < 0)
        {
            std::cerr << "Could not open decoder" << std::endl;
            return;
        }
    }

    std::vector<std::byte> getNextFrame()
    {
        AVPacket* pkt = av_packet_alloc();
        if (!pkt)
        {
            std::cerr << "Failed to allocate packet" << std::endl;
            return {};
        }

        if (av_read_frame(input_ctx, pkt) < 0)
        {
            std::cerr << "Failed to read frame" << std::endl;
            av_packet_free(&pkt);
            return {};
        }

        // Convert AVPacket data to std::vector<std::byte>
        std::vector<std::byte> frameData(pkt->size);
        memcpy(frameData.data(), pkt->data, pkt->size);

        av_packet_unref(pkt);
        av_packet_free(&pkt);

        return frameData;
    }

    std::vector<std::byte> encode(const std::vector<std::byte>& frame) override
    {
        AVPacket* pkt = av_packet_alloc();
        if (!pkt)
        {
            std::cerr << "Failed to allocate packet" << std::endl;
            return {};
        }

        AVFrame* av_frame = av_frame_alloc();
        if (!av_frame)
        {
            std::cerr << "Failed to allocate frame" << std::endl;
            av_packet_free(&pkt);
            return {};
        }

        // Setting frame properties - these should match your source and encoder settings
        av_frame->format = encoder_ctx->pix_fmt;
        av_frame->width = encoder_ctx->width;
        av_frame->height = encoder_ctx->height;

        // Read a frame from input (desktop capture)
        if (av_read_frame(input_ctx, pkt) < 0)
        {
            std::cerr << "Error reading frame from input" << std::endl;
            av_frame_free(&av_frame);
            av_packet_free(&pkt);
            return {};
        }

        // Send the frame to the encoder
        if (avcodec_send_frame(encoder_ctx, av_frame) < 0)
        {
            std::cerr << "Error sending frame to encoder" << std::endl;
            av_frame_free(&av_frame);
            av_packet_free(&pkt);
            return {};
        }

        // Receive the encoded frame
        if (avcodec_receive_packet(encoder_ctx, pkt) < 0)
        {
            std::cerr << "Error receiving packet from encoder" << std::endl;
            av_frame_free(&av_frame);
            av_packet_free(&pkt);
            return {};
        }

        // Copy encoded data to vector
        std::vector<std::byte> encoded_data(pkt->size);
        memcpy(encoded_data.data(), pkt->data, pkt->size);

        av_frame_free(&av_frame);
        av_packet_free(&pkt);

        return encoded_data;
    }


    std::vector<std::byte> decode(const std::vector<std::byte>& encodedFrame) override
    {
        AVPacket* pkt = av_packet_alloc();
        if (!pkt)
        {
            std::cerr << "Failed to allocate packet" << std::endl;
            return {};
        }

        // Copy encoded data to packet
        pkt->data = (uint8_t*) encodedFrame.data();
        pkt->size = encodedFrame.size();

        // Send the packet to the decoder
        if (avcodec_send_packet(decoder_ctx, pkt) < 0)
        {
            std::cerr << "Error sending packet to decoder" << std::endl;
            av_packet_free(&pkt);
            return {};
        }

        AVFrame* decoded_frame = av_frame_alloc();
        if (!decoded_frame)
        {
            std::cerr << "Failed to allocate frame" << std::endl;
            av_packet_free(&pkt);
            return {};
        }

        // Receive the decoded frame
        if (avcodec_receive_frame(decoder_ctx, decoded_frame) < 0)
        {
            std::cerr << "Error receiving frame from decoder" << std::endl;
            av_frame_free(&decoded_frame);
            av_packet_free(&pkt);
            return {};
        }

        // Convert decoded frame to a byte vector (or any suitable format for your application)
        std::vector<std::byte> decoded_data;
        memcpy(decoded_data.data(), decoded_frame->data, decoded_frame->linesize[0] * decoded_frame->height);


        av_frame_free(&decoded_frame);
        av_packet_free(&pkt);

        return decoded_data;
    }


    ~H265Encoder()
    {
        if (encoder_ctx)
        {
            avcodec_free_context(&encoder_ctx);
        }
        if (decoder_ctx)
        {
            avcodec_free_context(&decoder_ctx);
        }
        if (input_ctx)
        {
            avformat_close_input(&input_ctx);
        }
        avformat_network_deinit();
    }

private:
    AVCodecContext *encoder_ctx, *decoder_ctx;

public:
    AVFormatContext* input_ctx;
};