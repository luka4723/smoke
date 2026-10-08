#pragma once

#include "Grid2D.h"
#include <array>

class MAC2D {
public:
    Grid2D u;
    Grid2D v;

    int nx, ny;
    double h;

    MAC2D(int ny, int nx, double h);

    Grid2D divergence();
    std::array<double, 2> sampleVel(float y, float x); 
};