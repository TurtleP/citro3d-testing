#include "matrix.hpp"

#include "renderer.hpp"
#include "shader.hpp"

using namespace love;

void Matrix4::Multiply(const Matrix4& a, const Matrix4& b, C3D_Mtx& c)
{
    Mtx_Multiply(&c, &a.matrix, &b.matrix);
}

void Matrix4::Multiply(const Matrix4& a, const Matrix4& b, Matrix4& c)
{
    Multiply(a, b, c.matrix);
}

Matrix4::Matrix4()
{
    this->SetIdentity();
}

Matrix4::Matrix4(const C3D_Mtx& a)
{
    Mtx_Copy(&this->matrix, &a);
}

/* credit to @mtheall */
static bool fuzzy(float const a_, float const b_)
{
    float const EPSILON = 0.00001f;
    return fabs(a_ - b_) < EPSILON;
}

bool Matrix4::IsAffine2DTransform() const
{
    // z-vector must remain unchanged
    if (this->matrix.r[0].z != 0.0f || this->matrix.r[1].z != 0.0f || this->matrix.r[2].x != 0.0f ||
        this->matrix.r[2].y != 0.0f || this->matrix.r[2].z != 1.0f || this->matrix.r[2].w != 0.0f)
        return false;

    // last row must be {0, 0, 0, 1}
    if (this->matrix.r[3].x != 0.0f || this->matrix.r[3].y != 0.0f || this->matrix.r[3].z != 0.0f ||
        this->matrix.r[3].w != 1.0f)
        return false;

    float const a = this->matrix.r[0].x;
    float const b = this->matrix.r[0].y;
    float const c = this->matrix.r[1].x;
    float const d = this->matrix.r[1].y;

    // M * transpose(M) must be identity matrix, where M is upper-left 2x2 matrix
    if (!fuzzy(a * a + b * b, 1.0f) || !fuzzy(c * c + d * d, 1.0f) || !fuzzy(a * c + b * d, 0.0f))
        return false;

    return true;
}

bool Matrix4::IsAffine3DTransform() const
{
    // last row must be {0, 0, 0, 1}
    if (this->matrix.r[3].x != 0.0f || this->matrix.r[3].y != 0.0f || this->matrix.r[3].z != 0.0f ||
        this->matrix.r[3].w != 1.0f)
        return false;

    float const a = this->matrix.r[0].x;
    float const b = this->matrix.r[0].y;
    float const c = this->matrix.r[0].z;
    float const d = this->matrix.r[1].x;
    float const e = this->matrix.r[1].y;
    float const f = this->matrix.r[1].z;
    float const g = this->matrix.r[2].x;
    float const h = this->matrix.r[2].y;
    float const i = this->matrix.r[2].z;

    // M * transpose(M) must be identity matrix, where M is upper-left 3x3 matrix
    if (!fuzzy(a * a + b * b + c * c, 1.0f) || !fuzzy(d * d + e * e + f * f, 1.0f) ||
        !fuzzy(g * g + h * h + i * i, 1.0f) || !fuzzy(a * d + b * e + c * f, 0.0f) ||
        !fuzzy(a * g + b * h + c * i, 0.0f) || !fuzzy(d * g + e * h + g * i, 0.0f))
        return false;

    return true;
}
/* ----------------- */

Matrix4::Matrix4(const Matrix4& a, const Matrix4& b)
{
    Matrix4::Multiply(a, b, this->matrix);
}

Matrix4::Matrix4(float x, float y, float angle, float sx, float sy, float ox, float oy, float kx,
                 float ky)
{
    this->SetTransformation(x, y, angle, sx, sy, ox, oy, ky, ky);
}

void Matrix4::operator*=(const Matrix4& m)
{
    Matrix4::Multiply(*this, m, this->matrix);
}

Matrix4 Matrix4::operator*(const Matrix4& m) const
{
    return Matrix4(*this, m);
}

void Matrix4::SetIdentity()
{
    Mtx_Identity(&this->matrix);
}

void Matrix4::SetRawTransformation(float t00, float t10, float t01, float t11, float x, float y)
{
    Mtx_Zeros(&this->matrix);

    this->matrix.r[2].z = this->matrix.r[3].w = 1.0f;

    this->matrix.r[0].x = t00;
    this->matrix.r[1].x = t10;

    this->matrix.r[0].y = t01;
    this->matrix.r[1].y = t11;

    this->matrix.r[0].w = x;
    this->matrix.r[1].w = y;
}

void Matrix4::SetTransformation(float x, float y, float angle, float sx, float sy, float ox,
                                float oy, float kx, float ky)
{
    Mtx_Zeros(&this->matrix);

    /*
    ** Note that the offsets are applied before rotation, scaling, or shearing;
    ** scaling and shearing are applied before rotation.
    */
    float c = cosf(angle), s = sinf(angle);

    this->matrix.r[2].z = this->matrix.r[3].w = 1.0f;

    this->matrix.r[0].x = c * sx - ky * s * sy;                                    // = a
    this->matrix.r[1].x = s * sx + ky * c * sy;                                    // = b

    this->matrix.r[0].y = kx * c * sx - s * sy;                                    // = c
    this->matrix.r[1].y = kx * s * sx + c * sy;                                    // = d

    this->matrix.r[0].w = x - ox * this->matrix.r[0].x - oy * this->matrix.r[0].y; // = c
    this->matrix.r[1].w = y - ox * this->matrix.r[1].x - oy * this->matrix.r[1].y; // = d
}

void Matrix4::Translate(float x, float y)
{
    Mtx_Translate(&this->matrix, x, y, 0.0f, true);
}

void Matrix4::Rotate(float r)
{
    Mtx_RotateZ(&this->matrix, r, true);
}

void Matrix4::Scale(float sx, float sy)
{
    Mtx_Scale(&this->matrix, sx, sy, 1.0f);
}

void Matrix4::GetApproximateScale(float& sx, float& sy) const
{
    // clang-format off
    sx = sqrtf(this->matrix.r[0].x * this->matrix.r[0].x + this->matrix.r[0].y * this->matrix.r[0].y);
    sy = sqrtf(this->matrix.r[1].x * this->matrix.r[1].x + this->matrix.r[1].y * this->matrix.r[1].y);
    // clang-format on
}

void Matrix4::Shear(float kx, float ky)
{
    C3D_Mtx mtx;
    Mtx_Identity(&mtx);

    mtx.r[0].y = kx;
    mtx.r[1].x = ky;

    Mtx_Multiply(&this->matrix, &this->matrix, &mtx);
}

void Matrix4::TransformXY()
{
    auto target = Renderer::Instance().GetCurrent();
    Mtx_Copy(&target->GetModelView(), &this->matrix);
}

void Matrix4::TransformXY(const C3D_Mtx& elements)
{
    auto target = Renderer::Instance().GetCurrent();
    Mtx_Copy(&target->GetModelView(), &elements);
}

Matrix4 Matrix4::Inverse() const
{
    Matrix4 inv;
    Mtx_Copy(&inv.matrix, &this->matrix);

    Mtx_Inverse(&inv.matrix);

    return inv;
}

Matrix4 Matrix4::Ortho(float left, float right, float bottom, float top, float near, float far)
{
    Matrix4 ortho;
    Mtx_Ortho(&ortho.matrix, left, right, bottom, top, near, far, true);

    return ortho;
}
