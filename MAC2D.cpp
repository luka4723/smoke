#include "MAC2D.h"
#include <cmath>

MAC2D::MAC2D(int ny, int nx, double h) : u(ny, nx + 1, h), v(ny + 1, nx, h), nx(nx), ny(ny), h(h), solid(nx*ny,0){}

Grid2D MAC2D::divergence()
{
    Grid2D div(ny, nx, h);
    #pragma omp parallel for
    for (int i = 0; i < ny; i++) {
        for (int j = 0; j < nx; j++) {
            double val =(u.get(i, j+1) - u.get(i, j) + v.get(i+1, j) - v.get(i, j))/h;
            div.set(i, j, val);
        }
    }
    return div;
}

std::array<double, 2> MAC2D::sampleVel(float y, float x)
{
    std::array<double,2> result;
    result[0] = v.sample(y,x,V);
    result[1] = u.sample(y,x,U);
    return result;
}

void MAC2D::advectVel(float dt)
{   
    auto vel_src = *this;

    //v
    #pragma omp parallel for
    for (int i = 0; i < ny+1; i++) {
        double y = i * h;
        for (int j = 0; j < nx; j++) {
            double x = (j + 0.5) * h;
            auto vel = vel_src.sampleVel(y,x);

            //Remove comment for RK2
            vel = vel_src.sampleVel(y-vel[0]*dt/2,x-vel[1]*dt/2);

            float v_end = vel_src.v.sample(y-vel[0]*dt,x-vel[1]*dt,V);   
            v.set(i, j, v_end);
        }
    }

    //u
    #pragma omp parallel for
    for (int i = 0; i < ny; i++) {
        double y = (i + 0.5) * h;
        for (int j = 0; j < nx + 1; j++) {
            double x = j * h;
            auto vel = vel_src.sampleVel(y,x);

            //Remove comment for RK2
            vel = vel_src.sampleVel(y-vel[0]*dt/2,x-vel[1]*dt/2);

            float u_end = vel_src.u.sample(y-vel[0]*dt,x-vel[1]*dt,U);   
            u.set(i, j, u_end);
        }
    }
}

void MAC2D::addSources(Grid2D& grid) {
    for (Source& s : sources) {
        grid.set(s.i, s.j, s.val);
    }
}

void MAC2D::addBuoyancy(Grid2D& grid, float dt, double beta) {
    #pragma omp parallel for
    for (int i=1; i<ny; i++) {
        for (int j=0; j<nx; j++) {
            if (!isFluid(i-1,j) || !isFluid(i,j)) continue;
            double s = (grid.get(i-1,j) + grid.get(i,j)) / 2;
            double val = v.get(i,j) + dt*beta*s;
            v.set(i,j,val);
        }
    }
}

void MAC2D::addBuoyancy(Grid2D& density, Grid2D& temperature, float dt, double alpha, double beta) {
    #pragma omp parallel for
    for (int i=1; i<ny; i++) {
        for (int j=0; j<nx; j++) {
            if (!isFluid(i-1,j) || !isFluid(i,j)) continue;
            double s = (density.get(i-1,j) + density.get(i,j)) / 2;
            double T = (temperature.get(i-1,j) + temperature.get(i,j)) / 2;
            double val = v.get(i,j) + dt*(-alpha*s + beta*T);
            v.set(i,j,val);
        }
    }
}

void MAC2D::addVorticityConfinement(float dt, double eps)
{
    Grid2D w(ny, nx, h);
    Grid2D fx(ny, nx, h);
    Grid2D fy(ny, nx, h);

    #pragma omp parallel for
    for (int i=1; i<ny-1; i++) {
        for (int j=1; j<nx-1; j++) {
            if (!isFluid(i,j)) continue;
            double vr = (v.get(i,j+1) + v.get(i+1,j+1)) / 2;
            double vl = (v.get(i,j-1) + v.get(i+1,j-1)) / 2;
            double ut = (u.get(i+1,j) + u.get(i+1,j+1)) / 2;
            double ub = (u.get(i-1,j) + u.get(i-1,j+1)) / 2;
            w.set(i,j, (vr - vl)/(2*h) - (ut - ub)/(2*h));
        }
    }

    #pragma omp parallel for
    for (int i=1; i<ny-1; i++) {
        for (int j=1; j<nx-1; j++) {
            if (!isFluid(i,j)) continue;
            double gx = (std::abs(w.get(i,j+1)) - std::abs(w.get(i,j-1))) / (2*h);
            double gy = (std::abs(w.get(i+1,j)) - std::abs(w.get(i-1,j))) / (2*h);
            double len = std::sqrt(gx*gx + gy*gy) + 1e-20;
            fx.set(i,j, eps*h*(gy/len)*w.get(i,j));
            fy.set(i,j, -eps*h*(gx/len)*w.get(i,j));
        }
    }

    //u
    #pragma omp parallel for
    for (int i=0; i<ny; i++) {
        for (int j=1; j<nx; j++) {
            if (!isFluid(i,j-1) || !isFluid(i,j)) continue;
            double val = u.get(i,j) + dt*(fx.get(i,j-1) + fx.get(i,j))/2;
            u.set(i,j,val);
        }
    }

    //v
    #pragma omp parallel for
    for (int i=1; i<ny; i++) {
        for (int j=0; j<nx; j++) {
            if (!isFluid(i-1,j) || !isFluid(i,j)) continue;
            double val = v.get(i,j) + dt*(fy.get(i-1,j) + fy.get(i,j))/2;
            v.set(i,j,val);
        }
    }
}

void MAC2D::addCircleSource(double cx, double cy, double r, double value)
{
    for (int i = 0; i < ny; i++) {
        double y = (i + 0.5) * h;
        for (int j = 0; j < nx; j++) {
            double x = (j + 0.5) * h;
            double dx = x - cx;
            double dy = y - cy;
            if (dx*dx + dy*dy < r*r && isFluid(i, j)) sources.push_back({i, j, value});
        }
    }
}
