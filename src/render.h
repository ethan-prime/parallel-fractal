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

#include "colormap.h"

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

    void RenderBuddhabrot(i32 iters, i32 T, i64 samples, colormap_t colormap = {});
};