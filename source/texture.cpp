#include "texture.hpp"
#include "logfile.hpp"
#include "renderer.hpp"

#include <cstdio>

using namespace love;

Texture::Texture(const char* path)
{
    FILE* file = std::fopen(path, "rb");

    if (!file)
    {
        std::fclose(file);
        return;
    }

    std::fseek(file, 0, SEEK_END);
    long size = std::ftell(file);
    std::fseek(file, 0, SEEK_SET);

    std::unique_ptr<uint8_t[]> fdata = std::make_unique<uint8_t[]>(size);
    std::fread(fdata.get(), size, 1, file);

    this->texture           = new C3D_Tex();
    Tex3DS_Texture imported = Tex3DS_TextureImport(fdata.get(), size, this->texture, NULL, false);

    std::fclose(file);

    if (!imported)
        return;

    Tex3DS_TextureFree(imported);
    C3D_TexSetFilter(this->texture, GPU_LINEAR, GPU_LINEAR);

    this->width  = this->texture->width;
    this->height = this->texture->height;

    Quad::Viewport view { 0, 0, (double)this->width, (double)this->height };
    this->quad = new Quad { view, this->width, this->height };
}

Texture::~Texture()
{
    delete this->texture;
    delete this->quad;
}

void Texture::Draw(Graphics& graphics, const Matrix4& matrix)
{
    const auto& transform = graphics.GetTransform();
    bool is2D             = transform.IsAffine2DTransform();

    Matrix4 translated(transform, matrix);

    DrawCommand command(4);
    command.texture = this;

    if (is2D)
        translated.TransformXY(command.Positions().get(), this->quad->GetVertices(), command.count);

    const auto* coords = this->quad->GetTextureCoords();
    command.FillVertices(graphics.GetColor(), coords);

    Renderer::Instance().Render(command);
}