#include "texture.hpp"
#include "logfile.hpp"
#include "renderer.hpp"

#include <cstdio>

using namespace love;

Texture::Texture(const std::string& path) : valid(false)
{
    FILE* file = std::fopen(path.c_str(), "rb");

    if (!file)
    {
        std::fclose(file);
        return;
    }

    std::fseek(file, 0, SEEK_END);
    long size = std::ftell(file);
    std::rewind(file);

    auto fdata = std::make_unique<uint8_t[]>(size);
    auto read  = (long)std::fread(fdata.get(), 1, size, file);

    if (read != size)
    {
        std::fclose(file);
        return;
    }

    this->texture           = new C3D_Tex();
    Tex3DS_Texture imported = Tex3DS_TextureImport(fdata.get(), size, this->texture, NULL, false);

    std::fclose(file);

    if (!imported)
        return;

    const Tex3DS_SubTexture* subTexture = Tex3DS_GetSubTexture(imported, 0);

    this->width  = subTexture->width;
    this->height = subTexture->height;

    this->valid = true;

    Tex3DS_TextureFree(imported);

    this->texture->border = 0;
    C3D_TexSetFilter(this->texture, GPU_LINEAR, GPU_LINEAR);
    C3D_TexSetWrap(this->texture, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);

    Quad::Viewport view { 0, 0, (double)this->width, (double)this->height };
    this->quad = new Quad { view, this->texture->width, this->texture->height };
}

Texture::~Texture()
{
    if (this->quad)
        delete this->quad;
}

void Texture::Draw(Graphics& graphics, const Matrix4& matrix)
{
    if (!this->valid)
        return;

    const auto& transform = graphics.GetTransform();
    bool is2D             = transform.IsAffine2DTransform();

    Matrix4 translated(transform, matrix);

    DrawCommand command(4);
    command.handles = { this->texture };

    if (is2D)
        translated.TransformXY(command.Positions().get(), this->quad->GetVertices(), command.count);

    const auto* coords = this->quad->GetTextureCoords();
    command.FillVertices(graphics.GetColor(), coords);

    Renderer::Instance().Render(command);
}