#pragma once
#include <globals.h>


class DeNoise
{
public:
    DeNoise(std::vector<std::vector<Color3f>> img, int w, int h, Color3f maxS) : image(img), w(w), h(h), maxS(maxS) {}
    void ComputeCDF(std::vector<std::vector<Color3f>> img, int b);
    void ComputeLevels(std::vector<std::vector<Color3f>> noiseMap, int L, std::vector<std::vector<float>> out);

    void DeNoiseLevels(std::vector<std::vector<Color3f>> img, std::vector<Color3f> sigma,
                  std::vector<std::vector<std::vector<Color3f>>> out);

    void CombineLayers(std::vector<std::vector<std::vector<float>>> levelR,
                       std::vector<std::vector<std::vector<float>>> levelG,
                       std::vector<std::vector<std::vector<float>>> levelB,
                  std::vector<std::vector<Color3f>> noiseMap, std::vector<std::vector<float>> sigmas);

    void RemoveSpikes();

private:
    std::vector<std::vector<Color3f>> image;
    std::map<int, float> invNoiseCDF_R;
    std::map<int, float> invNoiseCDF_G;
    std::map<int, float> invNoiseCDF_B;
    Color3f maxS; // maximum stdev
    int w, h;
};

static inline void Histogram(std::vector<std::vector<Color3f>> img, int b, std::vector<std::vector<float>> out) {

}
