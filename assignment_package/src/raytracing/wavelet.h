#pragma once
#include <globals.h>

using namespace std;

float MedAbsDev(vector<vector<float>> img, vector<float> filter);
void HighPass(vector<float> &a, vector<float> &b, vector<float> &cx);

static inline void rowToCol(vector<vector<float>> in, vector<vector<float>> out) {
    int m = in.size();
    int n = in[0].size();
    for (int j = 0; j < n; j ++) {
        vector<float> col;
        for (int i = 0; i < m; i ++) {
            col.push_back(in[i][j]);
        }
        out.push_back(col);
    }
}
