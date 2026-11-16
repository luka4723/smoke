#include "recorder.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iostream>

#ifdef _WIN32
const char* NULL_DEV = "nul";
#else
const char* NULL_DEV = "/dev/null";
#endif

Recorder::Recorder(const std::string& dir, int scale, int fps) : dir(dir), scale(scale), fps(fps), frames(0)
{
    std::filesystem::create_directories(dir);
}

void Recorder::add(const std::string& name, Grid2D& g, double vmin, double vmax, Colormap cmap)
{
    std::filesystem::remove_all(dir + "/frames/" + name);
    tracks.push_back({name, &g, vmin, vmax, false, cmap});
}

void Recorder::add(const std::string& name, Grid2D& g, Colormap cmap)
{
    std::filesystem::remove_all(dir + "/frames/" + name);
    tracks.push_back({name, &g, 0.0, 0.0, true, cmap});
}

void Recorder::setSolid(const std::vector<char>& solid, int nx, int ny)
{
    mask.cells = &solid;
    mask.nx = nx;
    mask.ny = ny;
}

void Recorder::capture()
{
    #pragma omp parallel for
    for (int k = 0; k < static_cast<int>(tracks.size()); k++) {
        Track& t = tracks[k];
        if (t.autoRange && frames == 0) {
            auto [lo, hi] = std::minmax_element(t.grid->mat.begin(), t.grid->mat.end());
            t.vmin = *lo;
            t.vmax = *hi;
            if (t.cmap == Colormap::DIVERGING) {
                double m = std::max(std::abs(t.vmin), std::abs(t.vmax));
                t.vmin = -m;
                t.vmax = m;
            }
        }
        std::string path = framePath(dir + "/frames/" + t.name, frames);
        saveImage(*t.grid, path, t.vmin, t.vmax, t.cmap, scale, mask);
    }
    frames++;
}

void Recorder::finish()
{
    if (frames == 0) return;

    std::string check = std::string("ffmpeg -version > ") + NULL_DEV + " 2>&1";
    if (std::system(check.c_str()) != 0) {
        std::cout << "Recorder: ffmpeg not found, frames are in " << dir << "/frames/\n";
        return;
    }

    char last[16];
    std::snprintf(last, sizeof(last), "%04d", frames - 1);

    for (Track& t : tracks) {
        std::string in = dir + "/frames/" + t.name + "/frame_";
        std::string quiet = " -loglevel error -y ";

        std::string start = "ffmpeg" + quiet + "-i \"" + in + "0000.ppm\" \"" + dir + "/start_" + t.name + ".png\"";
        std::string end = "ffmpeg" + quiet + "-i \"" + in + last + ".ppm\" \"" + dir + "/end_" + t.name + ".png\"";
        std::string gif = "ffmpeg" + quiet + "-framerate " + std::to_string(fps) +
                          " -i \"" + in + "%04d.ppm\" -vf \"split[a][b];[a]palettegen[p];[b][p]paletteuse\" \"" +
                          dir + "/" + t.name + ".gif\"";

        int r = std::system(start.c_str());
        r += std::system(end.c_str());
        r += std::system(gif.c_str());
        if (r != 0) std::cout << "Recorder: ffmpeg error for " << t.name << "\n";
        std::cout << "Recorder: " << t.name << " (" << frames << " frames, range " << t.vmin << " .. " << t.vmax << ")\n";
    }
}
