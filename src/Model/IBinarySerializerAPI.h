//
// Created by Jonathan Richard on 2023-09-11.
//

#pragma once

#include <vector>

template<class T>
class IBinarySerializerAPI
{
public:
    virtual const std::vector<std::byte> Serialize(const T& object) = 0;
    virtual const T Deserialize(const std::vector<std::byte> data) = 0;
};
