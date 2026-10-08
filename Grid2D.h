#pragma once

#include <vector>

enum GridType {PRESSURE,U,V};

class Grid2D {
public:
    int ny, nx;
    double h;
    std::vector<double> mat;

    Grid2D(int ny, int nx, double h);

    double get(int n, int m);
    void set(int n, int m, double val);
    void print();
    void savePGM(const char* path, float lo, float hi);
    double sample(float y, float x, GridType t);
};