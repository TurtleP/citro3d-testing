#include "quad.hpp"

using namespace love;

Quad::Quad(const Viewport& viewport, double sourceWidth, double sourceHeight) :
    sourceWidth(sourceWidth),
    sourceHeight(sourceHeight)
{
    this->Refresh(viewport, sourceWidth, sourceHeight);
}

void Quad::Refresh(const Viewport& viewport, double sourceWidth, double sourceHeight)
{
    this->viewport     = viewport;
    this->sourceWidth  = sourceWidth;
    this->sourceHeight = sourceHeight;

    std::fill_n(this->vertices, 4, Vector2 {});

    this->vertices[0] = Vector2(0.0f, 0.0f);
    this->vertices[1] = Vector2(0.0f, (float)viewport.h);
    this->vertices[2] = Vector2((float)viewport.w, (float)viewport.h);
    this->vertices[3] = Vector2((float)viewport.w, 0.0f);

    std::fill_n(this->textureCoords, 4, Vector2 {});

    // clang-format off
    this->vertices[0] = Vector2((float)(viewport.x / sourceWidth), (float)(viewport.y / sourceHeight));
    this->vertices[1] = Vector2((float)(viewport.x / sourceWidth), (float)((viewport.y + viewport.h) / sourceHeight));
    this->vertices[2] = Vector2((float)((viewport.x + viewport.w) / sourceWidth), (float)((viewport.y + viewport.h) / sourceHeight));
    this->vertices[3] = Vector2((float)((viewport.x + viewport.w) / sourceWidth), (float)(viewport.y / sourceHeight));
    // clang-format on
}