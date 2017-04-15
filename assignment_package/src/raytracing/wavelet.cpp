#include "wavelet.h"

using namespace std;

struct Complex {
    float r;
    float i;
};

vector<float> kernel = vector<float> {-0.5f, -0.5f, 2.f, -0.5f, -0.5f};

bool sortR(Color3f i, Color3f j) { return i.r < j.r; }
bool sortG(Color3f i, Color3f j) { return i.g < j.g; }
bool sortB(Color3f i, Color3f j) { return i.b < j.b; }

Color3f MedAbsDev(vector<vector<Color3f>> img, Bounds2i bound)
{
    Point2i start = bound.Min();
    Point2i end = bound.Max();

    // row filter and downsample
    vector<vector<Color3f>> rowFiltered; // 4x8
    for (int i = start.x; i <= end.x; i = i + 2) {
        vector<Color3f> cx;
        HighPass(img[i], cx);
        rowFiltered.push_back(cx);
    }

    // column filter
    vector<vector<Color3f>> byCol;
    rowToCol(rowFiltered, byCol);
    vector<Color3f> arr;
    for (int i = start.y; i <= end.y; i = i + 2) {
        vector<Color3f> cx;
        HighPass(byCol[i], cx);
        for (int j = 0; j < cx.size(); j ++)
            arr.push_back(cx[j]);
    }

    // calculate median
    sort(arr.begin(), arr.end(), sortR); float r = arr[2].r;
    sort(arr.begin(), arr.end(), sortR); float g = arr[2].g;
    sort(arr.begin(), arr.end(), sortR); float b = arr[2].b;
    return Color3f(r,g,b) / 0.6745f;
}

// a: signal
// b: high pass filter
// cx: return
void HighPass(vector<Color3f> &a, vector<Color3f> &cx) {
    int len = a.size();
    for (int i = 0; i < len; i++) {
        int start = glm::max(0, i - 2);
        int end = glm::min(len - 1, i + 3);
        Color3f pass = Color3f(0.f);
        float delta = end - start;
        for (int j = start; j <= end; j++) pass += kernel[glm::abs(j - i)] * a[j] / delta;
        cx.push_back(glm::max(pass, Color3f(0.f)));
    }
}
