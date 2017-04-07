#pragma once

#include "shape.h"
#include "primitive.h"

struct CSGNode;
enum operation { OBJECT, UNION , INTER, DIFFER };

class CSG : public Primitive
{
public:
    CSG();
    ~CSG();
    Bounds3f WorldBound() const {
        Bounds3f bound;
        for (std::shared_ptr<Shape> s : shapes) bound = Union(bound, s->WorldBound());
        return bound;
    }
    bool Intersect(const Ray &ray, Intersection *isect) const;

    std::vector<std::shared_ptr<Shape>> shapes;
    std::vector<operation> operators;

private:
    CSGNode *recursiveBuild(int* shape_num, int* oper_num);
    CSGNode *root = nullptr;
};
