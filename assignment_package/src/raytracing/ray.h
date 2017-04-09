#pragma once
#include <globals.h>
//#include <scene/mediums/medium.h>

class Ray
{
public:
    Ray(const Point3f &o, const Vector3f &d);
    Ray(const glm::vec4 &o, const glm::vec4 &d);
    Ray(const Ray &r);

    //Return a copy of this ray that has been transformed
    //by the input transformation matrix.
    Ray GetTransformedCopy(const Matrix4x4 &T) const;
    Point3f operator() (float t) const {return origin + direction * t;}
    const Ray operator= (const Ray &r) const {return Ray(r);}

    Point3f origin;
    Vector3f direction;
    float tMax;
//    const Medium *medium;
};
