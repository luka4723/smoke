#pragma once

#include "Grid2D.h"
#include <string>
#include <vector>

enum class Colormap {
    SMOKE,     
    DIVERGING, 
    HEAT     
};

struct SolidMask {
    const std::vector<char>* cells = nullptr;
    int nx = 0;
    int ny = 0;
};

void saveImage(const Grid2D& g, const std::string& path,
               double vmin, double vmax,
               Colormap cmap = Colormap::SMOKE, int scale = 1,
               SolidMask mask = {});

void saveImageAuto(const Grid2D& g, const std::string& path,
                   Colormap cmap = Colormap::SMOKE, int scale = 1,
                   SolidMask mask = {});

std::string framePath(const std::string& dir, int frame,
                      const std::string& prefix = "frame");

// Dijagnostika
double maxAbs(const Grid2D& g); 
double sum(const Grid2D& g);  
