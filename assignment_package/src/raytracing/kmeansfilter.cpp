#include "kmeansfilter.h"

struct Bucket {
    Bucket() {total_color = Color3f(0.f); num = 0.f;}
    Color3f total_color;
    float num;
    Color3f GetAvg() {return total_color/num;}
};

K_MeansFilter::K_MeansFilter(std::vector<std::vector<Color3f>> p_colors,
                             std::vector<std::vector<bool>> has_color, int num)
    : sqrt_buckets(glm::sqrt(num)) {

    buckets = std::vector<std::vector<std::shared_ptr<Bucket>>>(sqrt_buckets);
    // initialize buckets
    for (int i = 0; i < sqrt_buckets; i++) {
        buckets[i] = std::vector<std::shared_ptr<Bucket>>(sqrt_buckets);
        for (int j = 0; j < sqrt_buckets; j++) {
            buckets[i][j] = std::make_shared<Bucket>();
        }
    }

    // populate buckets
    for (int i = 0; i < p_colors.size(); i++) {
        for (int j = 0; j < p_colors[i].size(); j++) {
            if (has_color[i][j]) {
                Vector2f hueSat = GetHueSat(p_colors[i][j]);
                int i_x = hueSat.x * sqrt_buckets / 360.f;
                if (i_x == sqrt_buckets) i_x --;
                int i_y = hueSat.y * sqrt_buckets;
                if (i_y == sqrt_buckets) i_y --;
                buckets[i_x][i_y]->num++;
                buckets[i_x][i_y]->total_color += p_colors[i][j];
            }

        }
    }
}

Color3f K_MeansFilter::Evaluate(Color3f c) {
    Vector2f hueSat = GetHueSat(c);
    int i_x = (int) hueSat.x * sqrt_buckets / 360.f;
    if (i_x == sqrt_buckets) i_x --;
    int i_y = (int) hueSat.y * sqrt_buckets;
    if (i_y == sqrt_buckets) i_y --;
    return buckets[i_x][i_y]->GetAvg();
}
