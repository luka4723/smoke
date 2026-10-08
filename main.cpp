#include "MAC2D.h"

int main()
{
    int nx = 64;
    int ny = 96;
    double h = 1.0 / nx;

    MAC2D velocity(ny, nx, h);

    for (int i = 0; i < ny; i++)
        for (int j = 0; j < nx + 1; j++)
            velocity.u.set(i, j, j * h);

    for (int i = 0; i < ny + 1; i++)
        for (int j = 0; j < nx; j++)
            velocity.v.set(i, j, 0.0);

    Grid2D grid = velocity.divergence();

    grid.savePGM("../test.pgm", 0, 1);

    return 0;
}