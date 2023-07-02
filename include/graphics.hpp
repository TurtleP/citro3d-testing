#pragma once

#include "color.hpp"
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

        struct DisplayState
        {
            Color foreground { 1, 1, 1, 1 };
            Color background { 0, 0, 0, 1 };

            float lineWidth     = 1.0f;
            LineJoin lineJoin   = LINE_JOIN_MITER;
            LineStyle lineStyle = LINE_SMOOTH;
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

      private:
        int CalculateEllipsePoints(float rx, float ry);

        std::vector<DisplayState> state;
        std::vector<Matrix4> transformStack;
        std::vector<double> pixelScaleStack;
    };
} // namespace love