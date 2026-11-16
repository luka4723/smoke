#include "advection.h"

void advectScalar(Grid2D& dst, Grid2D& src, MAC2D& velocity, float dt){
    #pragma omp parallel for
    for (int i = 0; i < dst.ny; i++) {
        double y = (i + 0.5) * dst.h;
        for (int j = 0; j < dst.nx; j++) {
            double x = (j + 0.5) * dst.h;
            
            auto vel = velocity.sampleVel(y,x);

            //Remove comment for RK2
            vel = velocity.sampleVel(y-vel[0]*dt/2,x-vel[1]*dt/2);

            float val_end = src.sample(y-vel[0]*dt,x-vel[1]*dt,SCALAR);
            dst.set(i, j, val_end);
        }
    }
}