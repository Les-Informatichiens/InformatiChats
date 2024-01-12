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
        const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_FFV1);
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
        encoder_ctx->pix_fmt = AV_PIX_FMT_BGRA;                                     // The decoded image format need to be compatible with BGRA or be reencoded(say for h265 the format is something like
                                                                                    // AV_PIX_FMT_YUV420P but the screengrab is in BGRA so we need to reencode it to YUV420P
        encoder_ctx->time_base = stream->time_base;                                 // Use the stream's time base
        encoder_ctx->framerate = av_inv_q(stream->time_base);                       // Inverse of the time bas

        if (avcodec_open2(encoder_ctx, codec, NULL) < 0)
        {
            std::cerr << "Could not open codec" << std::endl;
            return;
        }

        // Initialize decoder context for BMP
        const AVCodec* deccodec = avcodec_find_decoder(AV_CODEC_ID_BMP);

        decoder_ctx = avcodec_alloc_context3(deccodec);
        if (!decoder_ctx)
        {
            std::cerr << "Could not allocate video codec context" << std::endl;
            return;
        }
        decoder_ctx->width = stream->codecpar->width;
        decoder_ctx->height = stream->codecpar->height;
        decoder_ctx->pix_fmt = AV_PIX_FMT_BGRA;              // Doesnt matter, the decoder does what he wants for the output format, as per the documentation
        decoder_ctx->time_base = stream->time_base;          // Use the stream's time base
        decoder_ctx->framerate = av_inv_q(stream->time_base);// Inverse of the time base

        if (avcodec_open2(decoder_ctx, deccodec, NULL) < 0)
        {
            std::cerr << "Could not open decoder" << std::endl;
            return;
        }


        av_frame = av_frame_alloc();
        if (!av_frame)
        {
            std::cerr << "Failed to allocate frame" << std::endl;
            av_frame_unref(av_frame);
            return;
        }
        av_frame->format = decoder_ctx->pix_fmt;
        av_frame->width = decoder_ctx->width;
        av_frame->height = decoder_ctx->height;
    }

    // Read a frame from input (desktop capture), decode it and return it as a byte vector to be sent to the encode function afterwards.
    void grab_screen_frame()
    {
        AVPacket* pkt = av_packet_alloc();

        if (!pkt)
        {
            std::cerr << "Failed to allocate packet" << std::endl;
            return;
        }

        // Read a frame from input (desktop capture)
        if (av_read_frame(input_ctx, pkt) < 0)
        {

            std::cerr << "Error reading frame from input" << std::endl;
            av_packet_free(&pkt);
            return;
        }

        // Send the packet to the decoder
        if (avcodec_send_packet(decoder_ctx, pkt) < 0)
        {
            std::cerr << "Error sending packet to decoder" << std::endl;
            av_packet_free(&pkt);
            return;
        }

        //tests the resulting bmp, it works!
        //        FILE *fp = fopen("output.bmp", "wb");
        //        fwrite(pkt->data, 1, pkt->size, fp);
        //        fclose(fp);


        // Receive the decoded frame
        if (avcodec_receive_frame(decoder_ctx, av_frame) < 0)
        {
            std::cerr << "Error receiving frame from decoder" << std::endl;
            av_packet_free(&pkt);
            return;
        }

        av_packet_free(&pkt);
    }

    void encode() override
    {

        // Send the frame for encoding


        if (avcodec_send_frame(encoder_ctx, av_frame) < 0)
        {
            std::cerr << "Error sending a frame for encoding" << std::endl;
            return;
        }

        AVPacket* pkt = av_packet_alloc();
        if (!pkt)
        {
            std::cerr << "Failed to allocate packet" << std::endl;
            return;
        }

        // Receive the encoded frame
        if (avcodec_receive_packet(encoder_ctx, pkt) < 0)
        {
            std::cerr << "Error receiving packet from encoder" << std::endl;
            av_packet_free(&pkt);
            return;
        }

        av_frame_unref(av_frame);

        av_frame->format = decoder_ctx->pix_fmt;
        av_frame->width = decoder_ctx->width;
        av_frame->height = decoder_ctx->height;

        av_packet_free(&pkt);

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

        AVFrame* decoded_frame = av_frame;
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
            av_frame_unref(decoded_frame);
            av_packet_free(&pkt);
            return {};
        }

        // Convert decoded frame to a byte vector (or any suitable format for your application)
        std::vector<std::byte> decoded_data;
        decoded_data.resize(decoded_frame->linesize[0] * decoded_frame->height);
        memcpy(decoded_data.data(), decoded_frame->data, decoded_frame->linesize[0] * decoded_frame->height);


        av_packet_free(&pkt);

        return decoded_data;
    }


    ~H265Encoder()
    {
        if (av_frame)
        {
            av_frame_free(&av_frame);
        }
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
    AVFrame* av_frame;

public:
    AVFormatContext* input_ctx;
};