//
// Created by thierrycd on 2024-01-03.
//

#pragma once

#include <variant>

template <typename A, typename B>
constexpr bool hold_same_type(const A &a, const B &b)
{
    return std::visit(
            [](const auto &a, const auto &b){return std::is_same_v<decltype(a), decltype(b)>;},
            a, b
    );
}
