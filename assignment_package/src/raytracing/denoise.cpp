#include "denoise.h"

DeNoise::DeNoise(vector<vector<Color3f> > img,
                 vector<vector<Color3f> > sigma_sp,
                 Point2i dim) : noisy_image(img), w(dim.x), h(dim.y), win_size(8), g(3.f), buckets(1000) {

    Color3f max = SetNoiseMap(sigma_sp);

    // convert into 1D array format
    vector<vector<float>> flat_image = vector<vector<float>>(3);
    vector<float> imageR, imageG, imageB;
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            imageR.push_back(noisy_image[i][j].r);
            imageG.push_back(noisy_image[i][j].g);
            imageB.push_back(noisy_image[i][j].b);
        }
    }
    flat_image.push_back(imageR); flat_image.push_back(imageG); flat_image.push_back(imageB);

    vector<map<int, float>> map;
    ComputeCDF(max, map);

    vector<vector<float>> sigmas;
    ComputeLevels(map, sigmas);

    vector<vector<vector<float>>> out; // color, level, image
    DeNoiseLevels(flat_image, sigmas, out);

    vector<vector<float>> temp;
    CombineLayers(sigmas, out, temp);

    RemoveSpikes(temp);
}

// return max value
Color3f DeNoise::SetNoiseMap(vector<vector<Color3f>> sigma_sp)
{
    vector<vector<Color3f>> sigma_wp;
    Color3f max_wp = CalculateMedian(sigma_wp);

    Color3f max = Color3f(0.f);
    vector<float> R, G, B;
    for (int a = 0; a < w; a++) {
        for (int b = 0; b < h; b++) {
            Point2i start = Point2i(glm::max(0, a - 1), glm::max(0, b - 1));
            Point2i end = Point2i(glm::min(a + 1, w), glm::min(b + 1, w));
            Color3f max_sp, max_wp = Color3f(0.f);
            for (int i = start.x; i <= end.x; i++) {
                for (int j = start.y; j <= end.y; j++) {
                    max_sp = glm::max(sigma_sp[i][j], max_sp);
                    max_wp = glm::max(sigma_wp[i][j], max_wp);
                }
            }
            Color3f color = glm::pow(sigma_sp[a][b] / noisy_image[a][b], Vector3f(0.25f)) * sigma_wp[a][b];
            R.push_back(color.r); G.push_back(color.g); B.push_back(color.b);
            max = glm::max(color, max);
        }
    }

    L = glm::ceil(max_wp/ 10.f);

    // scale and normalize
    for (int j = 0; j < R.size(); j ++) {
        R[j] /= max.r; G[j] /= max.g; B[j] /= max.b;
        R[j] *= g * max_wp.r; G[j] *= g * max_wp.g; B[j] *= g * max_wp.b;
    }
    noiseMap.push_back(R); noiseMap.push_back(G); noiseMap.push_back(G);
    return max;
}

// return max wp
Color3f DeNoise::CalculateMedian(vector<vector<Color3f>> sigma_wp)
{
    float maxR, maxG, maxB = 0.f;
    for (int i = 0; i < w; i ++) {
        for (int j = 0; j < h; j ++) {

            Point2i start = Point2i(glm::max(0, i - win_size/2), glm::max(0, j - win_size/2));
            Point2i end = Point2i(glm::min(i + win_size/2, w), glm::min(j + win_size/2, h));
            vector<vector<float>> windowR, windowG, windowB;
            for (int a = start.x; a < end.x; a ++) {
                vector<float> winR, winG, winB;
                for (int b = start.y; b < end.y; b ++) {
                    winR.push_back(noisy_image[a][b].r); winG.push_back(noisy_image[a][b].g); winG.push_back(noisy_image[a][b].b);
                }
                windowR.push_back(winR); windowG.push_back(winG); windowB.push_back(winB);
            }
            sigma_wp[i][j].r = MedAbsDev(windowR, filter); maxR = glm::max(sigma_wp[i][j].r, maxR);
            sigma_wp[i][j].g = MedAbsDev(windowG, filter); maxG = glm::max(sigma_wp[i][j].g, maxG);
            sigma_wp[i][j].b = MedAbsDev(windowB, filter); maxB = glm::max(sigma_wp[i][j].b, maxB);
        }
    }
    return Color3f(maxR, maxG, maxB);
}

void DeNoise::ComputeCDF(Color3f max, vector<map<int, float>> out)
{
    out = vector<map<int, float>>(3);
    for (int c = 0; c < 3; c ++) {
        map<int, float> cdf_map;
        vector<int> hist = vector<int> (buckets);
        std::fill(hist.begin(), hist.end(), 0);
        for (int j = 0; j < noiseMap[c].size(); j ++) {
            int index = noiseMap[c][j] == max[c] ? buckets - 1 : noiseMap[c][j] * buckets / max[c];
            hist[index] ++;
        }
        float count = 0.f;
        for (int j = 0; j < buckets; j ++) {
            count += hist[j];
            cdf_map[count] = j * max[c] / buckets;
        }
        out.push_back(cdf_map);
    }
}

void DeNoise::ComputeLevels(vector<map<int, float>> cdf_map, vector<vector<float>> out) {
    out = vector<vector<float>>(3); // 3 sets of level sigmas
    Vector3f step, stepC = Vector3f(w*h) / L;

    for (int c = 0; c < 3; c ++) {
        vector<float> sigmas;
        auto i = cdf_map[c].begin();
        while (i != cdf_map[c].end()) {
            if (i->first >= step[c]) {
                sigmas.push_back(i->second); // save sigma
                stepC[c] += step[c];
            }
            i++;
        }
        out.push_back(sigmas);
    }
}

void DeNoise::DeNoiseLevels(
        vector<vector<float>> img_noisy, // color, image
        vector<vector<float>> sigmas, // color, level
        vector<vector<vector<float>>> out) // color, level, image
{

    // parameters
    const unsigned color_space = 3; // RGB
    const unsigned tau_2D_wien = 5; // BIOR or 4 for DCT
    const unsigned tau_2D_hard  = 5; // BIOR or 4 for DCT
    const bool useSD_1 = true; const bool useSD_2 = true;
    unsigned chnls = 1;

    // outputs
    for (int i = 0; i < 3; i ++) { // colors
        vector<vector<float>> levels;
        for (int j = 0; j < sigmas[i].size(); j++) { // levels
            vector<float> img_basic, img_denoised;

//            run_bm3d(sigmas[i][j], img_noisy, img_basic, img_denoised, w, h, chnls,
//                     useSD_1, useSD_2, tau_2D_hard, tau_2D_wien, color_space);

            levels.push_back(img_denoised);
        }
        out.push_back(levels);
    }
}

int index(vector<float> sigmas, float sigma) {
    for (int i = 0; i < sigmas.size(); i ++) {
        if (sigma > sigmas[i]) return i;
    }
}

void DeNoise::CombineLayers(vector<vector<float>> sigmas, vector<vector<vector<float>>> imgs, vector<vector<float>> out)
{
    out = vector<vector<float>>(3);
    for (int c = 0; c < sigmas.size(); c ++) { // for each color
        vector<float> color;
        for (int x = 0; x < noiseMap[c].size(); x ++) { // for each level (not uniform
            // interpolate between image levels
            float sigma = noiseMap[c][x]; int i_r = index(sigmas[c], sigma);
            float t = sigma - sigmas[c][i_r]/(sigmas[c][i_r + 1] - sigmas[c][i_r]);
            float r = glm::mix(imgs[c][i_r][x], imgs[c][i_r+1][x], t);
            color.push_back(r);
        }
        out.push_back(color);
    }
}

void DeNoise::RemoveSpikes(vector<vector<float>> in)
{
    // convert vector type
    for (int i = 0; i < w; i++) {
        vector<Color3f> row;
        for (int j = 0; j < h; j++) {
            int x = i*w + j;
            row.push_back(Color3f(in[0][x], in[1][x], in[2][x]));
        }
        denoised_image.push_back(row);
    }

    // calculate median, mean, std images
    vector<vector<Color3f>> mean, median, stdev;
    for (int a = 0; a < w; a ++) {
        vector<Color3f> rowMean, rowMed, rowSTD;
        for (int b = 0; b < h; b++) {
            vector<float> R, G, B;
            Color3f total_color = Color3f(0.f);
            vector<Color3f> all_colors;
            Point2i start = Point2i(glm::max(0, a - 1), glm::max(0, b - 1));
            Point2i end = Point2i(glm::min(a + 1, w - 1), glm::min(b + 1 , h - 1));
            for (int i = start.x; i <= end.x; i++) {
                for (int j = start.y; j <= end.y; j++) {
                    R.push_back(denoised_image[i][j].r);
                    G.push_back(denoised_image[i][j].g);
                    B.push_back(denoised_image[i][j].b);
                    total_color += denoised_image[i][j];
                    all_colors.push_back(denoised_image[i][j]);
                }
            }
            std::sort(R.begin(), R.end()); std::sort(G.begin(), G.end()); std::sort(G.begin(), G.end());
            rowMed.push_back(Color3f(R[3], G[3], B[3]));
            rowMean.push_back(total_color / 9.f);
            rowSTD.push_back(GetStdDev(all_colors, total_color / 9.f));
        }
        mean.push_back(rowMean); median.push_back(rowMed); stdev.push_back(rowSTD);
    }
    // replace all pixels whose (color - mean) > std with median
    for (int a = 0; a < w; a++) {
        for (int b = 0; b < h; b++) {
            if ((denoised_image[a][b].r - mean[a][b].r) > stdev[a][b].r) denoised_image[a][b].r = median[a][b].r;
            if ((denoised_image[a][b].g - mean[a][b].g) > stdev[a][b].g) denoised_image[a][b].g = median[a][b].g;
            if ((denoised_image[a][b].b - mean[a][b].b) > stdev[a][b].b) denoised_image[a][b].b = median[a][b].b;
        }
    }
}
