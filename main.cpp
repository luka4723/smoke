#include "MAC2D.h"
#include "advection.h"
#include <iostream>
#include <cmath>
#include "output.h"
#include "projection.h"
#include "recorder.h"

int main()
{
    int nx = 128;
    int ny = 128;
    double h = 1.0/nx;
    double dt = h;
    int steps = 400;
    double alpha = 0.3;   
    double beta = 1.5;    
    double cooling = 0.6; 
    double eps = 3.0;     
    MAC2D velocity(ny, nx, h);
    Grid2D src(ny, nx, h);
    Grid2D dst(ny, nx, h);
    Grid2D T(ny, nx, h);

    //adding solids
    for(int i=0; i<velocity.ny; i++){
        velocity.solid[velocity.nx*i] = 1;
        velocity.solid[velocity.nx*i + velocity.nx-1] = 1;
    }
    for (int j=0; j<velocity.nx; j++) {   
        velocity.solid[j] = 1;
        velocity.solid[(velocity.ny-1)*velocity.nx+j] = 1;
    }
    //varijanta: prepreka iznad izvora - dim se razdvaja oko nje
    for (int i = ny / 2; i < ny / 2 + 8; i++) {
       for (int j = nx / 2 - 12; j < nx / 2 + 12; j++) {
           velocity.solid[i * nx + j] = 1;
       }
    }
    
    //adding sources
    velocity.addCircleSource(0.5, 0.1, 0.05, 1);

    //v
    for (int i = 0; i < ny+1; i++) {
        double y = i * h;
        for (int j = 0; j < nx; j++) {
            double x = (j + 0.5) * h;
            //velocity.v.set(i, j, 0.05 * sin(4 * M_PI * x) * exp(-100 * (y - 0.5) * (y - 0.5)));
        }
    }

    //u
    for (int i = 0; i < ny; i++) {
        double y = (i + 0.5) * h;
        for (int j = 0; j < nx + 1; j++) {
            double x = j * h;
            //if (y > 0.5) velocity.u.set(i, j, 0.5);
            //else velocity.u.set(i, j, -0.5);
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
            //if (y < 0.5 && velocity.isFluid(i, j)) src.set(i, j, 1.0);
        }
    }

   //projekcija pocetnog polja
    Grid2D p(ny, nx, h);

    setWallVelocities(velocity);
    int it = project(velocity, p, dt, 1.0, 2000, 1e-8);
    std::cout << "max|div| = " << maxAbs(velocity.divergence()) << " (" << it << " iteracija)\n";
 
    auto temp = src;
    Grid2D div = velocity.divergence();
 
    Recorder rec("../tests");
    rec.setSolid(velocity.solid, nx, ny);
    rec.add("density", temp, 0.0, 1.0, Colormap::SMOKE);
    rec.add("u", velocity.u, -1.0, 1.0, Colormap::DIVERGING);
    rec.add("v", velocity.v, -1.0, 1.0, Colormap::DIVERGING);
    rec.add("pressure", p, Colormap::DIVERGING);
    rec.add("div", div, Colormap::DIVERGING);
    rec.add("temperature", T, 0.0, 1.0, Colormap::HEAT);
 
    rec.capture();
    for(int i=0;i<steps;i++)
    {
        velocity.addSources(temp);
        velocity.addSources(T);
        advectScalar(dst, temp, velocity, dt);
        temp = dst;
        advectScalar(dst, T, velocity, dt);
        T = dst;
        for (double& x : T.mat) x *= exp(-cooling*dt);
        velocity.advectVel(dt);
        velocity.addBuoyancy(temp, T, dt, alpha, beta);
        velocity.addVorticityConfinement(dt, eps);
        it = project(velocity, p, dt, 1.0, 500, 1e-5);

        if (i % 50 == 0) std::cout << "step " << i << ": " << it << " iteracija, max|v| = " << maxAbs(velocity.v) << "\n";
        div = velocity.divergence();
        rec.capture();
    }
    rec.finish();
 
    std::cout << ", kraj = " << sum(temp) << "\n";
    return 0;
}