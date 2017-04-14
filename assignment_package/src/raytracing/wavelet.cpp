#include "wavelet.h"

using namespace std;

struct Complex {
    float r;
    float i;
};

float MedAbsDev(vector<vector<float>> img, vector<float> filter) // img 8x8, diagonal 4x4
{
    unsigned int len_filter = filter.size();
    // row filter and downsample
    vector<vector<float>> rowFiltered; // 4x8
    for (int i = 0; i < img.size(); i = i + 2) {
        vector<float> cx;
        HighPass(img[i], filter, cx);
        cx.erase(cx.begin(), cx.begin()+len_filter);
        cx.erase(cx.begin()+img[0].size(),cx.end());
        rowFiltered.push_back(cx);
    }

    // column filter
    vector<vector<float>> byCol;
    rowToCol(rowFiltered, byCol);
    vector<float> arr;
    for (int i = 0; i < byCol.size(); i = i + 2) {
        vector<float> cx;
        HighPass(byCol[i], filter, cx);
        cx.erase(cx.begin(), cx.begin()+len_filter);
        cx.erase(cx.begin()+byCol[0].size(),cx.end());
        for (int j = 0; j < cx.size(); j ++) arr.push_back(cx[j]);
    }

    // calculate median
    sort(arr.begin(), arr.end());
    return (arr[3] + arr[4]) / 2.f / 0.6745f;
}

// a: signal
// b: high pass filter
// cx: return
void HighPass(vector<float> &a, vector<float> &b, vector<float> &cx) {
    unsigned int sz = a.size() + b.size() - 1;

    vector<Complex> inp_data = vector<Complex>(sx);
    vector<Complex> temp_data = vector<Complex>(sx);
    vector<Complex> filt_data = vector<Complex>(sx);

    for (unsigned int i =0; i < sz; i++) {
        if (i < a.size()) inp_data[i].r = a[i];
        else inp_data[i].r = 0.0;

        inp_data[i].i = 0.0;

        if (i < b.size()) filt_data[i].r = b[i];
        else filt_data[i].r = 0.0;

        filt_data[i].i = 0.0;
    }

    for (unsigned int i =0; i < sz; i++){
        temp_data[i].r = inp_data[i].r*filt_data[i].r - inp_data[i].i*filt_data[i].i;
        temp_data[i].i = inp_data[i].r*filt_data[i].i + inp_data[i].i*filt_data[i].r;
    }

    for (unsigned int i = 0; i < sz; i++) {
        double temp1;
        temp1 = temp_data[i].r / (float) sz;
        cx.push_back(temp1);
    }
}
