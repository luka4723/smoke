#include "MAC2D.h"


MAC2D::MAC2D(int ny, int nx, double h) : u(ny, nx + 1, h), v(ny + 1, nx, h), nx(nx), ny(ny), h(h), solid(nx*ny,0){}

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

bool MAC2D::isFluid(int i, int j) {
    return i >= 0 && i < ny && j >= 0 && j < nx && !solid[i * nx + j];
}
