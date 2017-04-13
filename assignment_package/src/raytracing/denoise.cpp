#include "denoise.h"

void DeNoise::ComputeCDF(std::vector<std::vector<Color3f> > noiseMap, int b) {
    std::vector<int> histR, histG, histB = std::vector<int> (b);
    std::fill(histR.begin(), histR.end(), 0);
    std::fill(histG.begin(), histG.end(), 0);
    std::fill(histB.begin(), histB.end(), 0);
    for (int i = 0; i < w; i ++) {
        for (int j = 0; j < h; j ++) {
            int i_r = noiseMap[i][j].r == maxS.r ? b - 1 : noiseMap[i][j].r * b / maxS.r;
            int i_g = noiseMap[i][j].g == maxS.g ? b - 1 : noiseMap[i][j].g * b / maxS.g;
            int i_b = noiseMap[i][j].b == maxS.b ? b - 1 : noiseMap[i][j].b * b / maxS.b;
            histR[i_r] ++; histR[i_g] ++; histR[i_b] ++;
        }
    }
    float countR, countG, countB = 0;
    for (int i = 0; i < b; i ++) {
        countR += histR[i]; countG += histG[i]; countB += histB[i];
        invNoiseCDF_R[countR] = i * maxS.r / b;
        invNoiseCDF_G[countG] = i * maxS.g / b;
        invNoiseCDF_B[countB] = i * maxS.b / b;
    }
}

void DeNoise::ComputeLevels(
        std::vector<std::vector<Color3f>> noiseMap,
        int L,
        std::vector<std::vector<float>> out) {
    out = std::vector<std::vector<float>>(3); // 3 2D color channels
    int stepR, stepG, stepB, step = w*h/L;
    auto i = invNoiseCDF_R.begin(), j = invNoiseCDF_G.begin(), k = invNoiseCDF_B.begin();
    while (i != invNoiseCDF_R.end()) {
        if (i->first >= stepR) {
            out[0].push_back(i->second); // save sigma
            stepR += step;
        }
        if (j->first >= stepG) {
            out[1].push_back(j->second);
            stepG += step;
        }
        if (k->first >= stepB) {
            out[2].push_back(k->second);
            stepB += step;
        }
        i++, j++, k++;
    }
}

void DeNoise::DeNoiseLevels(
        std::vector<std::vector<Color3f>> img,
        std::vector<Color3f> sigma,
        std::vector<std::vector<std::vector<Color3f>>> out) {

}

int index(std::vector<float> sigmas, float sigma) {
    for (int i = 0; i < sigmas.size(); i ++) {
        if (sigma > sigmas[i]) return i;
    }
}

void DeNoise::CombineLayers(
        std::vector<std::vector<std::vector<float>>> levelX, // vector of L 2D denoised for R channel
        std::vector<std::vector<std::vector<float>>> levelY, // vector of L 2D denoised for G channel
        std::vector<std::vector<std::vector<float>>> levelZ, // vector of L 2D denoised for B channel
        std::vector<std::vector<Color3f>> noiseMap, // 2D array of stdev
        std::vector<std::vector<float>> sigmas) { // 3 arrays of L sigma values
    for (int i = 0; i < w; i ++) {
        for (int j = 0; j < h; j ++) {
            float sigmaR = noiseMap[i][j].r; int i_r = index(sigmas[0], sigmaR);
            float sigmaG = noiseMap[i][j].g; int i_g = index(sigmas[1], sigmaG);
            float sigmaB = noiseMap[i][j].b; int i_b = index(sigmas[2], sigmaB);
            float t_r = sigmaR - sigmas[0][i_r]/(sigmas[0][i_r + 1] - sigmas[0][i_r]);
            float t_g = sigmaR - sigmas[1][i_g]/(sigmas[1][i_g + 1] - sigmas[1][i_g]);
            float t_b = sigmaR - sigmas[2][i_b]/(sigmas[2][i_b + 1] - sigmas[2][i_b]);
            float r = glm::mix(levelX[i_r][i][j], levelX[i_r+1][i][j], t_r);
            float g = glm::mix(levelY[i_g][i][j], levelY[i_g+1][i][j], t_g);
            float b = glm::mix(levelZ[i_b][i][j], levelZ[i_b+1][i][j], t_b);
            image[i][j] = Color3f(r, g, b);
        }
    }
}

void DeNoise::RemoveSpikes() {
    // calculate median, mean, std images
    std::vector<std::vector<Color3f>> mean, median, stdev;
    for (int a = 0; a < w; a++) {
        for (int b = 0; b < h; b++) {
            std::vector<float> R, G, B;
            Color3f total_color = Color3f(0.f);
            std::vector<Color3f> all_colors;
            Point2i start = Point2i(glm::max(0, a - 1), glm::max(0, b - 1));
            Point2i end = Point2i(glm::min(a + 1, w - 1), glm::min(b + 1 , h - 1));
            for (int i = start.x; i <= end.x; i++) {
                for (int j = start.y; j <= end.y; j++) {
                    R.push_back(image[i][j].r);
                    G.push_back(image[i][j].g);
                    B.push_back(image[i][j].b);
                    total_color += image[i][j];
                    all_colors.push_back(image[i][j]);
                }
            }
            std::sort(R.begin(), R.end()); std::sort(G.begin(), G.end()); std::sort(G.begin(), G.end());
            median[a][b] = Color3f(R[3], G[3], B[3]);
            mean[a][b] = total_color / 9.f;
            stdev[a][b] = GetStdDev(all_colors, mean[a][b]);
        }
    }
    // replace all pixels whose (color - mean) > std with median
    for (int a = 0; a < w; a++) {
        for (int b = 0; b < h; b++) {
            if ((image[a][b].r - mean[a][b].r) > stdev[a][b].r) image[a][b].r = median[a][b].r;
            if ((image[a][b].g - mean[a][b].g) > stdev[a][b].g) image[a][b].g = median[a][b].g;
            if ((image[a][b].b - mean[a][b].b) > stdev[a][b].b) image[a][b].b = median[a][b].b;
        }
    }
}
