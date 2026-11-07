#pragma once
#include "MAC2D.h"

extern std::vector<char> solid;

void setWallVelocities(MAC2D& vel);
int project(MAC2D& vel, Grid2D& p, double dt, double rho = 1.0,
            int maxIters = 2000, double tol = 1e-6);