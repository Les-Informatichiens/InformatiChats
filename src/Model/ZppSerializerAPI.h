//
// Created by Jonathan Richard on 2023-09-11.
//

#pragma once

#include <Model/IBinarySerializerAPI.h>
#include <zpp_bits.h>

template<class T>
class ZppSerializerAPI : IBinarySerializerAPI<T>
{
public:
    const std::vector<std::byte> Serialize(const T& object) override
    {
        auto [data, out] = zpp::bits::data_out();
        out(object).or_throw();
        return std::move(data);
    }

    const T Deserialize(const std::vector<std::byte> data) override
    {
        zpp::bits::in in(data);
        T o;
        in(o).or_throw();
        return std::move(o);
    }
};
