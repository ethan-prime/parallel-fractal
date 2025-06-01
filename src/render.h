#pragma once
typedef int i32;
typedef long i64;
typedef float f32;
typedef double f64;
typedef unsigned char channel;
#include <functional>
#include <complex.h>
#include <string>
#include <jpeglib.h>
#include <iostream>
#include <omp.h>
#include <random>
#include <cmath>
#define CF64 std::complex<double>

typedef struct color
{
    channel Red;
    channel Green;
    channel Blue;
} color_t;

typedef struct
{
    color_t color;
} pixel;

extern f64 GAMMA;

inline color_t wikipedia_colormap(f64 t)
{
    // Clamp t to [0,1]
    if (t < 0)
        t = 0;
    if (t > 1)
        t = 1;

    // For t == 1 (inside set), force dark blue
    if (t >= 1.0 - 1e-8)
        return color_t{10, 20, 80};

    // Apply gamma correction to stretch out low t values
    f64 t_gamma = std::pow(t, GAMMA);

    // Smooth polynomial coloring (Wikipedia style)
    f64 r = 9 * (1 - t_gamma) * t_gamma * t_gamma * t_gamma;
    f64 g = 15 * (1 - t_gamma) * (1 - t_gamma) * t_gamma * t_gamma;
    f64 b = 8.5 * (1 - t_gamma) * (1 - t_gamma) * (1 - t_gamma) * t_gamma + (1 - t_gamma) * 0.2;

    auto clamp = [](f64 x)
    {
        if (x < 0)
            return 0.0;
        if (x > 1)
            return 1.0;
        return x;
    };

    return color_t{
        static_cast<channel>(clamp(r) * 0xFF),
        static_cast<channel>(clamp(g) * 0xFF),
        static_cast<channel>(clamp(b) * 0xFF)};
}

typedef std::function<color_t(f64)> colormap_t;

class Image
{
private:
    i32 width;
    i32 height;
    f64 x1, y1, x2, y2;
    pixel *pixels;

public:
    Image(i32 width, i32 height, f64 x1, f64 y1, f64 x2, f64 y2);

    void Render(std::function<f64(CF64, CF64, i32)> fractal, CF64 c, i32 iters, i32 T, colormap_t colormap = {});
    void Save(const char *filename);
};