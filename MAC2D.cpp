#include "MAC2D.h"

MAC2D::MAC2D(int ny, int nx, double h) : u(ny, nx + 1, h), v(ny + 1, nx, h), nx(nx), ny(ny), h(h){}

Grid2D MAC2D::divergence()
{
    Grid2D div(ny, nx, h);
    for (int i = 0; i < ny; i++) {
        for (int j = 0; j < nx; j++) {
            double val =(u.get(i, j+1) - u.get(i, j) + v.get(i+1, j) - v.get(i, j))/h;
            div.set(i, j, val);
        }
    }
    return div;
}