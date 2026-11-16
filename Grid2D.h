#pragma once

#include <vector>

enum GridType {SCALAR,U,V};

class Grid2D {
public:
    int ny, nx;
    double h;
    std::vector<double> mat;

    Grid2D(int ny, int nx, double h);

    double get(int n, int m) {return mat[n * nx + m];}
    void set(int n, int m, double val) {mat[n * nx + m] = val;}
    void print();
    void savePGM(const char* path);
    double sample(float y, float x, GridType t);
    double min();
    double max();
};