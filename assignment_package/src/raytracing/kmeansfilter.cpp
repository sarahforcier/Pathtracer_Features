#include "kmeansfilter.h"

struct Bucket {
    Bucket() {total_color = Color3f(0.f); num = 0.f;}
    Color3f total_color;
    float num;
    Color3f GetAvg() {return total_color/num;}
};

K_MeansFilter::K_MeansFilter(std::vector<std::vector<Color3f>> p_colors, int num)
    : num_buckets(num) {
    buckets = std::vector<std::shared_ptr<Bucket>>(num_buckets);
    for (int i = 0; i < num_buckets; i++) buckets[i] = std::make_shared<Bucket>();

    for (int i = 0; i < p_colors.size(); i++) {
        for (int j = 0; j < p_colors[i].size(); j++) {
            if (true) {
                float hue = GetHue(p_colors[i][j]);
                int index = hue * 360.f / num_buckets;
                buckets[index]->num++;
                buckets[index]->total_color += p_colors[i][j];
            }

        }
    }
}

Color3f K_MeansFilter::Evaluate(Color3f c) {
    float hue = GetHue(c);
    int index = (int) hue * 360 / num_buckets;
    return buckets[index]->GetAvg();
}
