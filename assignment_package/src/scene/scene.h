#pragma once
#include <QList>
#include <raytracing/film.h>
#include <scene/camera.h>
#include <scene/lights/light.h>
#include <scene/geometry/shape.h>
#include "bvh.h"

class Primitive;
class BVHAccel;
class Material;
class Light;

class Scene
{
public:
    Scene();
    ~Scene();
    QList<std::shared_ptr<Primitive>> primitives;
    QList<std::shared_ptr<Material>> materials;
    QList<std::shared_ptr<Light>> lights;
    Camera camera;
    Film film;

    BVHAccel* bvh;

    QList<std::shared_ptr<Drawable>> drawables;

    void SetCamera(const Camera &c);

    void CreateTestScene();
    void Clear();

    bool Intersect(const Ray& ray, Intersection* isect) const;

    // returns first intersection with a light-scattering surface along the given ray
    // returns beam transmittance up to that point of intersection
    bool IntersectTr(const Ray& ray, Sampler &sampler, Intersection* isect, Color3f* Tr);

    void clearBVH();
};
