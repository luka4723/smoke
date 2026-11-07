#pragma once

#include "Grid2D.h"
#include <array>

struct Source { int i,j; double val;};

class MAC2D {
public:
    Grid2D u;
    Grid2D v;
    std::vector<char> solid;
    std::vector<Source> sources;

    int nx, ny;
    double h;

    MAC2D(int ny, int nx, double h);

    Grid2D divergence();
    std::array<double, 2> sampleVel(float y, float x); 
    void advectVel(float dt);
    bool isFluid(int i, int j);
    void addSources(Grid2D& grid);
    void addBuoyancy(Grid2D& grid, float dt, double beta);
    void addCircleSource(double cx, double cy, double r, double value);
};