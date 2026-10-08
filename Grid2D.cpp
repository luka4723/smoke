#include "Grid2D.h"

#include <iostream>
#include <fstream>

Grid2D::Grid2D(int ny, int nx, double h): ny(ny), nx(nx), h(h), mat(nx * ny, 0.0){}

double Grid2D::get(int n, int m) {return mat[n * nx + m];}

void Grid2D::set(int n, int m, double val){mat[n * nx + m] = val;}

void Grid2D::print()
{
    for (int i = 0; i < ny; i++) {
        for (int j = 0; j < nx; j++) std::cout << mat[i * nx + j] << " ";
        std::cout << "\n";
    }
}

void Grid2D::savePGM(const char* path, float lo, float hi)
{
    std::ofstream file(path);

    file << "P2\n";
    file << nx << " " << ny << "\n";
    file << "255\n";

    for (int j = ny - 1; j >= 0; --j) {
        for (int i = 0; i < nx; ++i) {
            float value = get(j, i);
            float t = (value - lo) / (hi - lo);

            if (t < 0.0f) t = 0.0f;
            if (t > 1.0f) t = 1.0f;

            int pixel = static_cast<int>(t * 255.0f);

            file << pixel << " ";
        }
        file << "\n";
    }
}