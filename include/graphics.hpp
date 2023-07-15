#pragma once

#include "color.hpp"
#include "exception.hpp"
#include "font.hpp"
#include "matrix.hpp"
#include "vector.hpp"

#include <span>
#include <vector>

namespace love
{
    class Graphics
    {
      public:
        enum DrawMode
        {
            DRAW_LINE,
            DRAW_FILL
        };

        enum ArcMode
        {
            ARC_OPEN,
            ARC_CLOSED,
            ARC_PIE
        };

        enum LineJoin
        {
            LINE_JOIN_NONE,
            LINE_JOIN_BEVEL,
            LINE_JOIN_MITER
        };

        enum LineStyle
        {
            LINE_SMOOTH,
            LINE_ROUGH
        };

        enum StackType
        {
            STACK_ALL,
            STACK_TRANSFORM
        };

        static constexpr auto MAX_USER_STACK_DEPTH = 0x80;

        struct DisplayState
        {
            Color foreground { 1, 1, 1, 1 };
            Color background { 0, 0, 0, 1 };

            float lineWidth     = 1.0f;
            LineJoin lineJoin   = LINE_JOIN_MITER;
            LineStyle lineStyle = LINE_SMOOTH;

            StrongReference<Font> font;
        };

        Graphics();

        static Graphics& Instance()
        {
            static Graphics graphics;
            return graphics;
        }

        void Polyline(const std::span<Vector2> points);

        void Polyfill(DrawMode mode, std::span<love::Vector2> points, const Color& color,
                      bool skipLastVertex = true);

        void Rectangle(DrawMode mode, float x, float y, float width, float height,
                       const Color& color);

        void Rectangle(DrawMode mode, float x, float y, float width, float height, float rx,
                       float ry, int points, const Color& color);

        void Ellipse(DrawMode mode, float x, float y, float a, float b, int points,
                     const Color& color);

        void Ellipse(DrawMode mode, float x, float y, float a, float b, const Color& color);

        void Circle(DrawMode mode, float x, float y, float radius, int points, const Color& color);

        void Circle(DrawMode mode, float x, float y, float radius, const Color& color);

        void Arc(DrawMode mode, ArcMode arcMode, float x, float y, float radius, float angle1,
                 float angle2, int points, const Color& color);

        void Arc(DrawMode mode, ArcMode arcMode, float x, float y, float radius, float angle1,
                 float angle2, const Color& color);

        void Print(const Font::ColoredStrings& strings, Font* font, const Matrix4& matrix);

        void Print(const Font::ColoredStrings& strings, const Matrix4& matrix);

        void Printf(const Font::ColoredStrings& strings, float wrap, Font::AlignMode align,
                    const Matrix4& matrix);

        void Printf(const Font::ColoredStrings& strings, Font* font, float wrap,
                    Font::AlignMode align, const Matrix4& matrix);

        void Origin()
        {
            this->transformStack.back().SetIdentity();
        }

        Matrix4& GetTransform()
        {
            return this->transformStack.back();
        }

        Color GetColor()
        {
            return this->state.back().foreground;
        }

        void SetColor(const Color& color)
        {
            this->state.back().foreground = color;
        }

        Color GetBackgroundColor()
        {
            return this->state.back().background;
        }

        void SetBackgroundColor(const Color& color)
        {
            this->state.back().background = color;
        }

        float GetLineWidth()
        {
            return this->state.back().lineWidth;
        }

        LineJoin GetLineJoin()
        {
            return this->state.back().lineJoin;
        }

        void SetLineJoin(LineJoin join)
        {
            this->state.back().lineJoin = join;
        }

        LineStyle GetLineStyle()
        {
            return this->state.back().lineStyle;
        }

        void SetLineStyle(LineStyle style)
        {
            this->state.back().lineStyle = style;
        }

        void SetFont(Font* font)
        {
            this->state.back().font = font;
        }

        Font* GetFont() const
        {
            return this->state.back().font;
        }

        void Push(StackType type = STACK_ALL)
        {
            if (this->stackTypeStack.size() == MAX_USER_STACK_DEPTH)
                throw love::Exception("Maximum stack depth reached (more pushes than pops?)");

            this->PushTransform();
            this->pixelScaleStack.push_back(this->pixelScaleStack.back());

            if (type == STACK_ALL)
                this->state.push_back(this->state.back());

            this->stackTypeStack.push_back(type);
        }

        void Pop()
        {
            if (this->stackTypeStack.size() < 1)
                throw love::Exception("Minimum stack depth reached (more pops than pushes?)");

            this->PopTransform();
            this->pixelScaleStack.pop_back();

            if (this->stackTypeStack.back() == STACK_ALL)
            {
                DisplayState& newState = this->state[this->state.size() - 2];
                this->RestoreStateChecked(newState);
                this->state.pop_back();
            }

            this->stackTypeStack.pop_back();
        }

        void RestoreStateChecked(const DisplayState& state)
        {
            const DisplayState& current = this->state.back();

            if (state.foreground != current.foreground)
                this->SetColor(state.foreground);

            this->SetBackgroundColor(state.background);

            /* todo set blend state */

            // this->SetLineWidth(state.lineWidth);
            this->SetLineStyle(state.lineStyle);
            this->SetLineJoin(state.lineJoin);

            // if (state.pointSize != current.pointSize)
            //     this->SetPointSize(state.pointSize);

            // this->SetMeshCullMode(state.cullMode);

            // if (state.windingMode != current.windingMode)
            //     this->SetFrontFaceWinding(state.windingMode);

            this->SetFont(state.font.Get());
            // this->SetShader(state.shader.Get());

            // if (state.colorMask != current.colorMask)
            //     this->SetColorMask(state.colorMask);

            // this->SetDefaultSamplerState(state.defaultSamplerState);
        }

        void Translate(float x, float y)
        {
            this->transformStack.back().Translate(x, y);
        }

        void Shear(float kx, float ky)
        {
            this->transformStack.back().Shear(kx, ky);
        }

        void Rotate(float r)
        {
            this->transformStack.back().Rotate(r);
        }

        void Scale(float x, float y)
        {
            this->transformStack.back().Scale(x, y);
            this->pixelScaleStack.back() *= (fabs(x) + fabs(y)) / 2.0;
        }

        void PushTransform()
        {
            this->transformStack.push_back(this->transformStack.back());
        }

        void PopTransform()
        {
            this->transformStack.pop_back();
        }

      private:
        int CalculateEllipsePoints(float rx, float ry);

        std::vector<DisplayState> state;
        std::vector<Matrix4> transformStack;
        std::vector<double> pixelScaleStack;
        std::vector<StackType> stackTypeStack;
    };
} // namespace love