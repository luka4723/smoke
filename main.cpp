#include "MAC2D.h"
#include "advection.h"
#include <iostream>
#include <math.h>
#include "output.h"

int main()
{
    int nx = 64;
    int ny = 64;
    double h = 1.0 / nx;
    double dt = h;
    int steps = 30;
    double omega = 2*M_PI/(steps*h);

    MAC2D velocity(ny, nx, h);
    Grid2D src(ny, nx, h);
    Grid2D dst(ny, nx, h);
    
    //v
    for (int i = 0; i < ny+1; i++) {
        double y = i * h;
        for (int j = 0; j < nx; j++) {
            double x = (j + 0.5) * h;
            velocity.v.set(i, j, -0.1);
        }
    }

    //u
    for (int i = 0; i < ny; i++) {
        double y = (i + 0.5) * h;
        for (int j = 0; j < nx + 1; j++) {
            double x = j * h;
            velocity.u.set(i, j, 0.2);
        }
    }

    //scalar
    for (int i = 0; i < ny; i++) {
        double y = (i + 0.5) * h;
        for (int j = 0; j < nx; j++) {
            double x = (j + 0.5) * h;
            
            double dx = x - 0.5;
            double dy = y - 0.25;
            double r = 0.1;
            if (dx*dx + dy*dy < r*r) src.set(i, j, 1.0);
        }
    }

    auto temp = src;
    for(int i=0;i<steps;i++) 
    {
        saveImage(temp, framePath("frames", i), 0.0, 1.0, Colormap::SMOKE, 4);
        advectScalar(dst, temp, velocity, dt);
        temp = dst;
    }
    //std::cout << velocity.sampleVel(0.2,0.3)[0];
    Grid2D div = velocity.divergence();

    std::cout << "max |div| = " << maxAbs(div) << "\n";
    std::cout << "kolicina dima: pocetak = " << sum(src) << ", kraj = " << sum(temp) << "\n";
 
    // jednokratne slike (opseg se racuna automatski)
    saveImageAuto(div, "../tests/div.ppm", Colormap::DIVERGING, 4);
    saveImageAuto(src, "../tests/src.ppm", Colormap::SMOKE, 4);
    saveImageAuto(temp, "../tests/dst.ppm", Colormap::SMOKE, 4);
    return 0;
}