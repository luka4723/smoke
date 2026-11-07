#include "MAC2D.h"
#include "advection.h"
#include <iostream>
#include <math.h>
#include "output.h"
#include "projection.h"
#include <cstdlib>
#include "recorder.h"

int main()
{
    int nx = 128;
    int ny = 128;
    double h = 1.0 / nx;
    double dt = h;
    int steps = 1000;

    MAC2D velocity(ny, nx, h);
    Grid2D src(ny, nx, h);
    Grid2D dst(ny, nx, h);
    for(int i=0; i<velocity.ny; i++){
        velocity.solid[velocity.nx*i] = 1;
        velocity.solid[velocity.nx*i + velocity.nx-1] = 1;
    }
    for (int j=0; j<velocity.nx; j++) {   
        velocity.solid[j] = 1;
        velocity.solid[(velocity.ny-1)*velocity.nx+j] = 1;
    }
    
    //v
    for (int i = 0; i < ny+1; i++) {
        double y = i * h;
        for (int j = 0; j < nx; j++) {
            double x = (j + 0.5) * h;
            velocity.v.set(i, j, 0.05 * sin(4 * M_PI * x) * exp(-100 * (y - 0.5) * (y - 0.5)));
        }
    }

    //u
    for (int i = 0; i < ny; i++) {
        double y = (i + 0.5) * h;
        for (int j = 0; j < nx + 1; j++) {
            double x = j * h;
            if (y > 0.5) velocity.u.set(i, j, 0.5);
            else velocity.u.set(i, j, -0.5);
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
            if (y < 0.5 && velocity.isFluid(i, j)) src.set(i, j, 1.0);
        }
    }

   //projekcija pocetnog polja
    Grid2D p(ny, nx, h);
    setWallVelocities(velocity);
    int it = project(velocity, p, dt, 1.0, 20000, 1e-8);
    std::cout << "max|div| = " << maxAbs(velocity.divergence()) << " (" << it << " iteracija)\n";
 
    auto temp = src;
    Grid2D div = velocity.divergence();
 
    Recorder rec("../tests");
    rec.add("density", temp, 0.0, 1.0, Colormap::SMOKE);
    rec.add("u", velocity.u, Colormap::DIVERGING);
    rec.add("v", velocity.v, Colormap::DIVERGING);
    rec.add("pressure", p, Colormap::DIVERGING);
    rec.add("div", div, Colormap::DIVERGING);
 
    for(int i=0;i<steps;i++)
    {
        div = velocity.divergence();
        rec.capture();
        advectScalar(dst, temp, velocity, dt);
        temp = dst;
        velocity.advectVel(dt);
        project(velocity, p, dt, 1.0, 500, 1e-5);
    }
    div = velocity.divergence();
    rec.capture();
    rec.finish();
 
    std::cout << "kolicina dima: pocetak = " << sum(src) << ", kraj = " << sum(temp) << "\n";
    return 0;
}