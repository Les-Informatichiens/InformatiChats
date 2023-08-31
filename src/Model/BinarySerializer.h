//
// Created by Jean on 8/28/2023.
//
#pragma once

#include <any>
#include <stdexcept>
#include <utility>
#include <vector>

template<typename T>
class BinarySerializer
{
public:
    const std::byte* Serialize(const T& object) const
    {
        return reinterpret_cast<std::byte const* const>(&object);
    }

    T DeSerialize(const std::byte*& serializedData)
    {
        T deserializedObj;
        if (sizeof(serializedData) != sizeof(T))
        {
            throw std::runtime_error("Serialized data size mismatch.");
        }

        std::memcpy(&deserializedObj, serializedData, sizeof(T));
        return deserializedObj;
    }
};
