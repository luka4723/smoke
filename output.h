#pragma once

#include "Grid2D.h"
#include <string>

enum class Colormap {
    SMOKE,     
    DIVERGING, 
    HEAT       
};

void saveImage(const Grid2D& g, const std::string& path,
               double vmin, double vmax,
               Colormap cmap = Colormap::SMOKE, int scale = 1);


void saveImageAuto(const Grid2D& g, const std::string& path,
                   Colormap cmap = Colormap::SMOKE, int scale = 1);

std::string framePath(const std::string& dir, int frame,
                      const std::string& prefix = "frame");

double maxAbs(const Grid2D& g); 
double sum(const Grid2D& g);
