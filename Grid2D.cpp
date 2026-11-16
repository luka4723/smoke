#include "Grid2D.h"

#include <iostream>
#include <fstream>
#include <math.h>
#include <algorithm>

Grid2D::Grid2D(int ny, int nx, double h): ny(ny), nx(nx), h(h), mat(nx * ny, 0.0){}

void Grid2D::print()
{
    for (int i = 0; i < ny; i++) {
        for (int j = 0; j < nx; j++) std::cout << mat[i * nx + j] << " ";
        std::cout << "\n";
    }
}

void Grid2D::savePGM(const char* path)
{
    std::ofstream file(path);

    file << "P2\n";
    file << nx << " " << ny << "\n";
    file << "255\n";

    for (int j = 0; j <ny; j++) {
        for (int i = 0; i < nx; ++i) {
            float value = get(j, i);
            float t = (value - this->min()) / (this->max() - this->min());

            if (t < 0.0f) t = 0.0f;
            if (t > 1.0f) t = 1.0f;

            int pixel = static_cast<int>(t * 255.0f);

            file << pixel << " ";
        }
        file << "\n";
    }
}

double Grid2D::sample(float y, float x, GridType t)
{
    float off_x,off_y;
    if(t == SCALAR) {off_x=0.5f;off_y=0.5f;}
    else if(t == U) {off_x=0.0f;off_y=0.5f;}
    else if(t == V) {off_x=0.5f;off_y=0.0f;}

    float grid_x = x/h - off_x;
    float grid_y = y/h - off_y;

    grid_x = std::clamp(grid_x, 0.0f, static_cast<float>(nx - 1));
    grid_y = std::clamp(grid_y, 0.0f, static_cast<float>(ny - 1));

    int x0,x1;
    x0 = floor(grid_x);
    if (x0 == nx - 1) {
        x0--;
        x1 = nx - 1;
    }
    else x1 = x0 + 1;
    int y0,y1;
    y0 = floor(grid_y);
    if (y0 == ny - 1) {
        y0--;
        y1 = ny - 1;
    }
    else y1 = y0 + 1;

    double hor1 = this->get(y0,x0)*(x1-grid_x)/(x1-x0)+this->get(y0,x1)*(grid_x-x0)/(x1-x0); 
    double hor2 = this->get(y1,x0)*(x1-grid_x)/(x1-x0)+this->get(y1,x1)*(grid_x-x0)/(x1-x0);

    return hor1 * (y1-grid_y)/(y1-y0) + hor2 * (grid_y-y0)/(y1-y0);
}

double Grid2D::max() {return *std::max_element(mat.begin(), mat.end());}
double Grid2D::min() {return *std::min_element(mat.begin(), mat.end());}