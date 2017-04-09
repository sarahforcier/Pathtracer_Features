#pragma once

class PhaseFunction
{
public:
    PhaseFunction();

    // samples an incident direction wi and a sample value in [0, 1)^2
    // does not return pdf since a call to p() will work
    virtual float Sample_p(const Vector3f &wo, Vector3f *wi, const Point2f &u) const = 0;
};
