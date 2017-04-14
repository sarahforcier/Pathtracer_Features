#pragma once
#include <globals.h>
//#include "bm3d.h"
#include "wavelet.h"

using namespace std;

class DeNoise
{
public:
    DeNoise(vector<vector<Color3f>> img, vector<vector<Color3f>> sigma_sp, Point2i dim);

    Color3f CalculateMedian(vector<vector<Color3f>> sigma_wp);

    Color3f SetNoiseMap(vector<vector<Color3f>> sigma_wp);

    void ComputeCDF(Color3f max, vector<map<int, float>> out); // map: int = CDF, float = sigma
    void ComputeLevels(vector<map<int, float>> in, vector<vector<float>> out);

    void DeNoiseLevels(vector<vector<float>> img_noisy, // converted original image
                       vector<vector<float>> sigmas,
                       vector<vector<vector<float>>> out);

    // write to image
    void CombineLayers(vector<vector<float>> sigmas,
                       vector<vector<vector<float>>> in,
                       vector<vector<float>> out);

    void RemoveSpikes(vector<vector<float>> in);
    vector<vector<Color3f>> denoised_image;


private:
    vector<vector<Color3f>> noisy_image; // original image
    vector<vector<float>> noiseMap; // A 1D array of normalized standard deviation

    Color3f L;
    int w, h, buckets, win_size;
    float g;

    vector<float> filter;
};
