#pragma once
#include <globals.h>
#include "filt.h"

using namespace std;

Color3f MedAbsDev(vector<vector<Color3f>> img, int w, int h, Point2i pixel);
void HighPass(vector<Color3f> &a,vector<Color3f> &cx);

static inline void rowToCol(vector<vector<Color3f>> in, vector<vector<Color3f>> &out)
{

    int m = in.size();
    int n = in[0].size();
    for (int j = 0; j < n; j ++) {
        vector<Color3f> col;
        for (int i = 0; i < m; i ++) {
            col.push_back(in[i][j]);
        }
        out.push_back(col);
    }
}
