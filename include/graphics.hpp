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

        struct DisplayState
        {
            Color foreground { 1, 1, 1, 1 };
            Color background { 0, 0, 0, 1 };
        };

        Graphics();

        static Graphics& Instance()
        {
            static Graphics graphics;
            return graphics;
        }

        void Polyfill(std::span<love::Vector2> points, const Color& color,
                      bool skipLastVertex = true);

        void Rectangle(float x, float y, float width, float height, const Color& color);

        void Rectangle(float x, float y, float width, float height, float rx, float ry, int points,
                       const Color& color);

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

        Color GetBackgroundColor()
        {
            return this->state.back().background;
        }

      private:
        int CalculateEllipsePoints(float rx, float ry);

        std::vector<DisplayState> state;
        std::vector<Matrix4> transformStack;
    };
} // namespace love