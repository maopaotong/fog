#pragma once

namespace fog
{
    struct Math
    {
        static constexpr float PI = 3.14159265358979323846;
        static constexpr double SQRT3 = 1.7320508075688772;

        //
        template <typename F, typename... Args>
        static void forEachPointOnCircle(float size, float offsetAngle, F &&visit, Args... args)
        {

            // rad=1, RAD=2
            float angle = 360.0f / size;

            for (int i = 0; i < size; i++)
            {

                float angle_rad = (angle * i + offsetAngle) * PI / 180.0f;

                float dx = std::cos(angle_rad);
                float dy = std::sin(angle_rad);

                visit(i, Vector2(dx, dy), args...);
            }
        };
        template <int Deg>
        static constexpr double radian()
        {
            return Deg * PI / 180;
        }

    };
};