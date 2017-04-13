#include <raytracing/film.h>
#include <QImage>
#include <QColor>

void Film::PostProcess() {
   std::vector<std::vector<Color3f>> sigma_wp;
   Color3f maxWP = CalculateMedian(sigma_wp);
   Color3f max = SetNoiseMap(sigma_wp);
    for (int i = 0; i < bounds.Max().x; i++) {
        for (int j = 0; j < bounds.Max().y; j++) {
            noiseMap[i][j] /= max;
            noiseMap[i][j] += 3.f * maxWP;
        }
    }
}

void Film::SetDimensions(unsigned int w, unsigned int h)
{
    bounds = Bounds2i(Point2i(0,0), Point2i(w, h));
    pixels.clear();
    pixels = std::vector<std::vector<Color3f>>(w);
    noiseMap = std::vector<std::vector<Color3f>>(w);
    for(unsigned int i = 0; i < w; i++){
        pixels[i] = std::vector<Color3f>(h);
        noiseMap[i] = std::vector<Color3f>(h);
    }
    pixel_has_color = std::vector<std::vector<bool>>(w, std::vector<bool>(h, false));
}

void Film::SetPixelColor(const Point2i &pixel, const Color3f pixelColor)
{
    pixels[pixel.x][pixel.y] = pixelColor;
    pixel_has_color[pixel.x][pixel.y] = true;
}

void Film::SetSigma_Sp(const Point2i &pixel, const Color3f stdev)
{
    sigma_sp[pixel.x][pixel.y] = stdev;
}

Color3f Film::GetColor(const Point2i &pixel)
{
    return pixels[pixel.x][pixel.y];
}

// return max value
Color3f Film::SetNoiseMap(std::vector<std::vector<Color3f>> sigma_wp) {
    Color3f max = Color3f(0.f);
    for (int a = 0; a < bounds.Max().x; a++) {
        for (int b = 0; b < bounds.Max().y; b++) {
            Point2i start = Point2i(glm::max(0, a - 1), glm::max(0, b - 1));
            Point2i end = Point2i(glm::min(a + 1, bounds.Max().x), glm::min(b + 1, bounds.Max().x));
            Color3f max_sp, max_wp = Color3f(0.f);
            for (int i = start.x; i <= end.x; i++) {
                for (int j = start.y; j <= end.y; j++) {
                    max_sp = glm::max(sigma_sp[i][j], max_sp);
                    max_wp = glm::max(sigma_wp[i][j], max_wp);
                }
            }
            noiseMap[a][b] = glm::pow(sigma_sp[a][b] / pixels[a][b], Vector3f(0.25f)) * sigma_wp[a][b];
            max = glm::max(noiseMap[a][b], max);
        }
    }
}

// return max wp
Color3f Film::CalculateMedian(std::vector<std::vector<Color3f>> sigma_wp) {
    sigma_wp = std::vector<std::vector<Color3f>>(bounds.Max().x);
    Color3f max = Color3f(0.f);
    for(unsigned int i = 0; i < bounds.Max().x; i++) sigma_wp[i] = std::vector<Color3f>(bounds.Max().y);
    for (int a = 0; a < bounds.Max().x; a++) {
        for (int b = 0; b < bounds.Max().y; b++) {
            std::vector<float> R, G, B;
            Point2i start = Point2i(glm::max(0, a - 3), glm::max(0, b - 3));
            Point2i end = Point2i(glm::min(a + 3, bounds.Max().x - 1), glm::min(b + 3, bounds.Max().y - 1));
            for (int i = start.x; i <= end.x; i++) {
                for (int j = start.y; j <= end.y; j++) {
                    R.push_back(pixels[i][j].r);
                    G.push_back(pixels[i][j].g);
                    B.push_back(pixels[i][j].b);
                }
            }
            std::sort(R.begin(), R.end()); std::sort(G.begin(), G.end()); std::sort(G.begin(), G.end());
            sigma_wp[a][b] = Color3f(R[3], G[3], B[3]) / 0.6745f;
            max = glm::max(sigma_wp[a][b], max);
        }
    }
    return max;
}

void Film::WriteImage(QString path)
{
    if(QString::compare(path.right(4), QString(".png"), Qt::CaseInsensitive) != 0)
    {
        path.append(QString(".png"));
    }

    Vector2i dims = bounds.Diagonal();

    QImage output(dims.x, dims.y, QImage::Format_RGB32);

    for(unsigned int i = 0; i < dims.x; i++)
    {
        for(unsigned int j = 0; j < dims.y; j++)
        {
            Color3f color = (pixels[i][j]) * 255.f;
            output.setPixel(i, j, QColor(color.r, color.g, color.b).rgb());
        }
    }
    output.save(path);
}

bool Film::IsPixelColorSet(const Point2i &pixel)
{
    return pixel_has_color[pixel.x][pixel.y];
}

void Film::cleanPixels() {
    Vector2i diag = this->bounds.Diagonal();
    for(unsigned int i = 0; i < diag.x; ++i)
        pixels.at(i).clear();
    pixels.clear();
    SetDimensions(diag.x, diag.y);
}

