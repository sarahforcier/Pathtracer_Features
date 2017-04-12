#pragma once
#include <globals.h>

// sort by hue
struct Bucket;

class K_MeansFilter
{
public:
    K_MeansFilter(std::vector<std::vector<Color3f>> p_colors,
                  std::vector<std::vector<bool>> has_color, int num);
    ~K_MeansFilter() {}

    Color3f Evaluate(Color3f c);

private:
    const int sqrt_buckets;
    std::vector<std::vector<std::shared_ptr<Bucket>>> buckets; // H by S
};

static inline Vector2f GetHueSat(Color3f c) {
    float min = glm::min(c.r, glm::min(c.g, c.b));
    float max = glm::max(c.r, glm::max(c.g, c.b));
    float delta = max - min;
    if (min == max) return Vector2f(0.f);
    float l = (min + max) / 2.f;
    float d = (c.r == max) ? c.g - c.b :
                ((c.b == max) ? c.r - c.g : c.b - c.r);
    float h = (c.r == max) ? 0.f :
                ((c.b == max) ? 4.f : 2.f);
    return Vector2f(60.f * (d/delta + h), delta/(1.f - glm::abs(2.f*l - 1.f)));
}
