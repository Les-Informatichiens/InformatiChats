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
    virtual void decode() = 0;

    virtual ~IEncoder() = default;
};
