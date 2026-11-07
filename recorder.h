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
    SolidMask mask;

    Recorder(const std::string& dir, int scale = 4, int fps = 30);

    // Fiksan opseg vrednosti
    void add(const std::string& name, Grid2D& g, double vmin, double vmax, Colormap cmap);
    // Opseg se uzima iz prvog frejma i ostaje isti do kraja
    void add(const std::string& name, Grid2D& g, Colormap cmap);

    // Cvrste celije se na svim slikama crtaju braon
    void setSolid(const std::vector<char>& solid, int nx, int ny);

    void capture();
    void finish();
};
