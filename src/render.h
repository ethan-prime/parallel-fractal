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
#include <cmath> // for sin/cos in colormap
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

/*typedef union {
    // julia function here, or a ptr idk
} fractal_t;*/

typedef std::function<color_t(f64)> colormap_t;

// Example: Wikipedia-style smooth blue-yellow-red colormap
#define GAMMA 0.5
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
    f64 gamma = GAMMA; // < 1.0 increases contrast near 0
    f64 t_gamma = std::pow(t, gamma);

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

class Image
{
private:
    i32 width;
    i32 height;
    i32 x1, y1, x2, y2;
    pixel *pixels;

public:
    Image(i32 width, i32 height, i32 x1, i32 y1, i32 x2, i32 y2)
    {
        this->width = width;
        this->height = height;
        this->pixels = new pixel[width * height];
        this->x1 = x1;
        this->y1 = y1;
        this->x2 = x2;
        this->y2 = y2;
    };

    void Render(std::function<f64(CF64, CF64, i32)> fractal, CF64 c, i32 iters, i32 T, colormap_t colormap = nullptr)
    {
        // antialiasing: average 16 samples per pixel
        const int samples = 16;
#pragma omp parallel for num_threads(T) schedule(dynamic) collapse(2)
        for (int y = 0; y < this->height; y++)
        {
            for (int x = 0; x < this->width; x++)
            {
                // Thread-safe random generator
                unsigned int seed = static_cast<unsigned int>(x + y * width) ^ static_cast<unsigned int>(omp_get_thread_num());
                std::mt19937 rng(seed);
                std::uniform_real_distribution<f64> dist(0.0, 1.0);

                f64 sum = 0.0;
                for (int s = 0; s < samples; ++s)
                {
                    // Thread-safe random offset in [0,1)
                    f64 dx = dist(rng);
                    f64 dy = dist(rng);
                    f64 fx = (static_cast<f64>(x) + dx) / (width - 1);
                    f64 fy = (static_cast<f64>(y) + dy) / (height - 1);
                    f64 px = x1 + (x2 - x1) * fx;
                    f64 py = y2 - (y2 - y1) * fy;
                    CF64 z(px, py);
                    f64 res = fractal(z, c, iters);
                    // Clamp res to [0,1]
                    if (res < 0)
                        res = 0;
                    if (res > 1)
                        res = 1;
                    sum += res;
                }
                f64 avg = sum / samples;
                color_t color;
                if (colormap)
                    color = colormap(avg);
                else
                {
                    channel val = static_cast<channel>(avg * 0xFF);
                    color = color_t{val, val, val};
                }
                this->pixels[y * width + x].color = color;
            }
        }
    }

    void Save(const char *filename)
    {
        // Save as JPEG using libjpeg
        FILE *fp = fopen(filename, "wb");
        if (!fp)
            return;

        jpeg_compress_struct cinfo;
        jpeg_error_mgr jerr;
        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_compress(&cinfo);
        jpeg_stdio_dest(&cinfo, fp);

        cinfo.image_width = width;
        cinfo.image_height = height;
        cinfo.input_components = 3;
        cinfo.in_color_space = JCS_RGB;

        jpeg_set_defaults(&cinfo);
        jpeg_set_quality(&cinfo, 95, TRUE);

        jpeg_start_compress(&cinfo, TRUE);

        JSAMPROW row_pointer[1];
        unsigned char *row = new unsigned char[width * 3];

        while (cinfo.next_scanline < cinfo.image_height)
        {
            for (int x = 0; x < width; ++x)
            {
                color_t &c = pixels[cinfo.next_scanline * width + x].color;
                row[x * 3 + 0] = c.Red;
                row[x * 3 + 1] = c.Green;
                row[x * 3 + 2] = c.Blue;
            }
            row_pointer[0] = row;
            jpeg_write_scanlines(&cinfo, row_pointer, 1);
        }

        delete[] row;
        jpeg_finish_compress(&cinfo);
        jpeg_destroy_compress(&cinfo);
        fclose(fp);
    }
};