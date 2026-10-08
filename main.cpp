#include "MAC2D.h"
#include <iostream>

int main()
{
    int nx = 64;
    int ny = 96;
    double h = 1.0 / nx;

    MAC2D velocity(ny, nx, h);

    for (int i = 0; i < ny; i++) {
        double y = (i + 0.5) * h;
        for (int j = 0; j < nx + 1; j++) {
            double x = j * h;
            velocity.u.set(i, j, 2 * x + 3 * y);
        }
    }

    for (int i = 0; i < ny+1; i++) {
        double y = i * h;
        for (int j = 0; j < nx; j++) {
            double x = (j + 0.5) * h;
            velocity.v.set(i, j, 2 * x + 3 * y);
        }
    }

    std::cout << velocity.sampleVel(0.2,0.3)[0];
    Grid2D grid = velocity.divergence();

    grid.savePGM("../test.pgm", 0, 1);

    return 0;
}