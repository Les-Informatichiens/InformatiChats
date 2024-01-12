//
// Created by thierrycd on 2024-01-08.
//

#pragma once

enum class VideoEncoding
{
    // VP8,
    // VP9,
    // H264,
    H265,
    // AV1
};


class IEncoder
{
public:
    virtual void encode() = 0;
    virtual std::vector<std::byte> decode(const std::vector<std::byte>& encodedFrame) = 0;

    virtual ~IEncoder() = default;
};
