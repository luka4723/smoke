#pragma once

#include "Grid2D.h"
#include "output.h"
#include <string>
#include <vector>

struct Track {
    std::string name;
    Grid2D* grid;
    double vmin, vmax;
    bool autoRange;
    Colormap cmap;
};

class Recorder {
public:
    std::string dir;
    int scale;
    int fps;
    int frames;
    std::vector<Track> tracks;

    Recorder(const std::string& dir, int scale = 4, int fps = 30);

    void add(const std::string& name, Grid2D& g, double vmin, double vmax, Colormap cmap);
    void add(const std::string& name, Grid2D& g, Colormap cmap);

    void capture();
    void finish();
};
