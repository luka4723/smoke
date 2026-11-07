#pragma once

#include "Grid2D.h"
#include <array>

class MAC2D {
public:
    Grid2D u;
    Grid2D v;
    std::vector<char> solid;

    int nx, ny;
    double h;

    MAC2D(int ny, int nx, double h);

    Grid2D divergence();
    std::array<double, 2> sampleVel(float y, float x); 
    void advectVel(float dt);
    bool isFluid(int i, int j);
};