#pragma once

#include "vector.hpp"

#include <array>

#include <citro3d.h>

namespace vertex
{
    struct Vertex
    {
        std::array<float, 3> position;
        std::array<float, 4> color;
        std::array<uint16_t, 2> texcoord;
    };

    static inline uint16_t normto16t(float in)
    {
        return uint16_t(in * 0xFFFF);
    }

    static constexpr size_t VERTEX_SIZE = sizeof(Vertex);

    static std::array<uint16_t, 2> Normalize(const love::Vector2& in)
    {
        return { normto16t(in.x), normto16t(in.y) };
    }
}