#include "output.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace {

struct RGB { uint8_t r, g, b; };

uint8_t toByte(double t) {
    return static_cast<uint8_t>(std::clamp(t, 0.0, 1.0) * 255.0 + 0.5);
}

RGB applyColormap(double t, Colormap cmap) {
    t = std::clamp(t, 0.0, 1.0);
    switch (cmap) {
    case Colormap::SMOKE: {
        double s = std::pow(t, 0.6);
        return {toByte(s), toByte(s), toByte(s)};
    }
    case Colormap::DIVERGING: {
        if (t < 0.5) {
            double s = t / 0.5;
            return {toByte(0.23 + 0.77 * s), toByte(0.30 + 0.70 * s), toByte(0.75 + 0.25 * s)};
        }
        double s = (t - 0.5) / 0.5;
        return {toByte(1.0 - 0.30 * s), toByte(1.0 - 0.85 * s), toByte(1.0 - 0.85 * s)};
    }
    case Colormap::HEAT: {
        double r = std::clamp(3.0 * t, 0.0, 1.0);
        double g = std::clamp(3.0 * t - 1.0, 0.0, 1.0);
        double b = std::clamp(3.0 * t - 2.0, 0.0, 1.0);
        return {toByte(r), toByte(g), toByte(b)};
    }
    }
    return {0, 0, 0};
}

const RGB SOLID_COLOR = {120, 80, 45};

bool maskSolid(const SolidMask& m, int i, int j)
{
    if (i < 0 || i >= m.ny || j < 0 || j >= m.nx) return true;
    return (*m.cells)[static_cast<size_t>(i) * m.nx + j] != 0;
}

bool isSolidPixel(const Grid2D& g, const SolidMask& m, int i, int j)
{
    if (m.cells == nullptr) return false;
    if (g.ny == m.ny && g.nx == m.nx) return maskSolid(m, i, j);                                 // centri celija
    if (g.ny == m.ny && g.nx == m.nx + 1) return maskSolid(m, i, j - 1) && maskSolid(m, i, j);  // u lica
    if (g.ny == m.ny + 1 && g.nx == m.nx) return maskSolid(m, i - 1, j) && maskSolid(m, i, j);  // v lica
    return false;
}

}

void saveImage(const Grid2D& g, const std::string& path,
               double vmin, double vmax, Colormap cmap, int scale, SolidMask mask)
{
    scale = std::max(scale, 1);
    const double range = vmax - vmin;
    const int W = g.nx * scale;
    const int H = g.ny * scale;

    std::vector<uint8_t> pixels(static_cast<size_t>(W) * H * 3);
    for (int py = 0; py < H; py++) {
        const int i = g.ny - 1 - py / scale;
        for (int px = 0; px < W; px++) {
            const int j = px / scale;
            const double val = g.mat[static_cast<size_t>(i) * g.nx + j];
            const double t = (range > 0.0) ? (val - vmin) / range : 0.5;
            const RGB c = isSolidPixel(g, mask, i, j) ? SOLID_COLOR : applyColormap(t, cmap);
            const size_t k = (static_cast<size_t>(py) * W + px) * 3;
            pixels[k] = c.r;
            pixels[k + 1] = c.g;
            pixels[k + 2] = c.b;
        }
    }

    std::ofstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "saveImage: can't open " << path << "\n";
        return;
    }
    file << "P6\n" << W << " " << H << "\n255\n";
    file.write(reinterpret_cast<const char*>(pixels.data()), pixels.size());
}

void saveImageAuto(const Grid2D& g, const std::string& path, Colormap cmap, int scale, SolidMask mask)
{
    auto [lo, hi] = std::minmax_element(g.mat.begin(), g.mat.end());
    double vmin = *lo, vmax = *hi;
    if (cmap == Colormap::DIVERGING) {
        double m = std::max(std::abs(vmin), std::abs(vmax));
        vmin = -m;
        vmax = m;
    }
    saveImage(g, path, vmin, vmax, cmap, scale, mask);
}

std::string framePath(const std::string& dir, int frame, const std::string& prefix)
{
    std::filesystem::create_directories(dir);
    char name[64];
    std::snprintf(name, sizeof(name), "%s_%04d.ppm", prefix.c_str(), frame);
    return (std::filesystem::path(dir) / name).string();
}

double maxAbs(const Grid2D& g)
{
    double m = 0.0;
    for (double x : g.mat) m = std::max(m, std::abs(x));
    return m;
}

double sum(const Grid2D& g)
{
    double s = 0.0;
    for (double x : g.mat) s += x;
    return s;
}
