#pragma once
#include "filter.h"

class MitchellFilter : public Filter
{
public:
    MitchellFilter(const Vector2f &radius, float B, float C)
        : Filter(radius), B(B), C(C) {}
    float Evaluate(const Point2f &p) const {
        return Mitchell1D(p.x*invRadius.x) * Mitchell1D(p.y*invRadius.y);
    }
    float Mitchell1D(float x) const {
        x = glm::abs(2.f * x);
        if (x > 1)
            return ((-B - 6*C)*x*x*x + (6*B + 30*C)*x*x +
                    (-12*B - 48*C)*x + (8*B + 24*C))*(1.f/6.f);
        else
            return ((12 - 9*B - 6*C)*x*x*x + (-18 + 12*B + 6*C)*x*x +
                    (6 - 2*B))*(1.f/6.f);
    }
private:
    const float B,C;
};
