#include "render.h"

Image::Image(i32 width, i32 height, f64 x1, f64 y1, f64 x2, f64 y2)
{
    this->width = width;
    this->height = height;
    this->pixels = new pixel[width * height];
    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;
}

void Image::Render(std::function<f64(CF64, CF64, i32)> fractal, CF64 c, i32 iters, i32 T, colormap_t colormap)
{

    // most of the work is done here!!!
    // this is where we can parallelize
    const int samples = 16;
#pragma omp parallel for num_threads(T) schedule(dynamic) collapse(2)
    for (int y = 0; y < this->height; y++)
    {
        for (int x = 0; x < this->width; x++)
        {
            unsigned int seed = static_cast<unsigned int>(x + y * width) ^ static_cast<unsigned int>(omp_get_thread_num());
            std::mt19937 rng(seed);
            std::uniform_real_distribution<f64> dist(0.0, 1.0);

            f64 sum = 0.0;
            for (int s = 0; s < samples; ++s)
            {
                f64 dx = dist(rng);
                f64 dy = dist(rng);
                f64 fx = (static_cast<f64>(x) + dx) / (width - 1);
                f64 fy = (static_cast<f64>(y) + dy) / (height - 1);
                f64 px = x1 + (x2 - x1) * fx;
                f64 py = y2 - (y2 - y1) * fy;
                CF64 z(px, py);
                f64 res = fractal(z, c, iters);
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

void Image::Save(const char *filename)
{
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
