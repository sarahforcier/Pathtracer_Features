#pragma once
#include <globals.h>

// sort by hue
struct Bucket;

class K_MeansFilter
{
public:
    K_MeansFilter(std::vector<std::vector<Color3f>> p_colors, int num);
    ~K_MeansFilter() {}

    Color3f Evaluate(Color3f c);

private:
    const int num_buckets;
    std::vector<std::shared_ptr<Bucket>> buckets;
};

static inline float GetHue(Color3f c) {
    float min = glm::min(c.r, glm::min(c.g, c.b));
    float max = glm::max(c.r, glm::max(c.g, c.b));
    float delta = max - min;
    if (min == max) return 0;
    float d = (c.r == min) ? c.g - c.b :
                ((c.b == min) ? c.r - c.g : c.b - c.r);
    float h = (c.r == min) ? 3.f :
                ((c.b == min) ? 1.f : 5.f);
    return 60.f * (h - d/delta);
}
