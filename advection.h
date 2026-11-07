#pragma once

#include "MAC2D.h"

void advectScalar(Grid2D& dst, Grid2D& src, MAC2D& velocity, float dt);