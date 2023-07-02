#pragma once

#include "vector.hpp"

#include <array>

#include <citro3d.h>

namespace vertex
{
    enum TriangleIndexMode
    {
        TRIANGLE_NONE,
        TRIANGLE_STRIP,
        TRIANGLE_FAN,
        TRIANGLE_QUADS,
        TRIANGLE_TRIS
    };

    enum PrimitiveType
    {
        PRIMITIVE_TRIANGLES,
        PRIMITIVE_TRIANGLE_STRIP,
        PRIMITIVE_TRIANGLE_FAN,
        PRIMITIVE_QUADS,
        PRIMITIVE_POINTS,
        PRIMITIVE_MAX_ENUM
    };

    struct Vertex
    {
        std::array<float, 3> position;
        std::array<float, 4> color;
        std::array<float, 2> texcoord;
    };

    static inline GPU_Primitive_t GetMode(PrimitiveType mode)
    {
        switch (mode)
        {
            case PRIMITIVE_TRIANGLE_FAN:
                return GPU_TRIANGLE_FAN;
            case PRIMITIVE_QUADS:
                return GPU_TRIANGLE_FAN;
            case PRIMITIVE_TRIANGLE_STRIP:
                return GPU_TRIANGLE_STRIP;
            default:
                return GPU_TRIANGLES;
        }
    }

    static inline uint16_t normto16t(float in)
    {
        return uint16_t(in * 0xFFFF);
    }

    static constexpr size_t VERTEX_SIZE = sizeof(Vertex);

    static inline std::array<uint16_t, 2> Normalize(const love::Vector2& in)
    {
        return { normto16t(in.x), normto16t(in.y) };
    }
} // namespace vertex