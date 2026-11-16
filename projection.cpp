#include "projection.h"
#include <cmath>
#include <algorithm>

void setWallVelocities(MAC2D& vel)
{
    for(int i=0; i<vel.ny; i++){
        for(int j=0; j<vel.nx; j++){    
            if (vel.solid[i*vel.nx + j]) {
                vel.u.set(i,j,0);   
                vel.u.set(i,j+1,0);
                vel.v.set(i,j,0);   
                vel.v.set(i+1,j,0);  
            }
        }
    }
}

int project(MAC2D& vel, Grid2D& p, double dt, double rho, int maxIters, double tol)
{
    int nx = vel.nx;
    int ny = vel.ny;
    double h = vel.h;

    setWallVelocities(vel);

    Grid2D div = vel.divergence();
    double scale = rho*h*h / dt;

    int it = 0;
    for (; it < maxIters; it++) {
        double maxChange = 0.0;
        // red-black ordering
        for (int color=0; color<2; color++) {
            #pragma omp parallel for reduction(max:maxChange)
            for (int i=0; i<ny; i++) {
                for (int j=(i+color)%2; j<nx; j+=2) {
                    if (!vel.isFluid(i,j)) continue;

                    int n = 0;
                    double sumNb = 0.0;
                    if (vel.isFluid(i,j-1)) { n++; sumNb += p.get(i,j-1); }
                    if (vel.isFluid(i,j+1)) { n++; sumNb += p.get(i,j+1); }
                    if (vel.isFluid(i-1,j)) { n++; sumNb += p.get(i-1,j); }
                    if (vel.isFluid(i+1,j)) { n++; sumNb += p.get(i+1,j); }
                    if (n == 0) continue;

                    double newP = (sumNb - scale*div.get(i,j))/n;
                    maxChange = std::max(maxChange, std::abs(newP - p.get(i,j)));
                    p.set(i,j,newP);
                }
            }
        }
        if (maxChange < tol) break;
    }

    double k = dt/(rho*h);

    //u
    #pragma omp parallel for
    for (int i=0; i<ny; i++) {
        for (int j=1;j<nx; j++) {
            if (vel.isFluid(i,j-1) && vel.isFluid(i,j)) {
                double val = vel.u.get(i,j) - k*(p.get(i,j) - p.get(i,j-1));
                vel.u.set(i,j,val);
            }
        }
    }

    //v
    #pragma omp parallel for
    for (int i=1; i<ny; i++) {
        for (int j=0; j<nx; j++) {
            if (vel.isFluid(i-1,j) && vel.isFluid(i,j)) {
                double val = vel.v.get(i,j) - k*(p.get(i,j) - p.get(i-1,j));
                vel.v.set(i,j,val);
            }
        }
    }

    return it;
}