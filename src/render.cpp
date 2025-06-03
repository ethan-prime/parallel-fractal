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

void Image::RenderBuddhabrot(i32 iters, i32 T, i64 samples, colormap_t colormap)
{
    const int supersample = 2; // 2x supersampling (render at 2x resolution)
    i32 ss_width = this->width * supersample;
    i32 ss_height = this->height * supersample;

    const i64 max_dim = 65500;
    if (ss_width > max_dim || ss_height > max_dim)
    {
        std::cerr << "Maximum supported image dimension is " << max_dim << " pixels\n";
        return;
    }

    std::vector<std::vector<i64>> hist(ss_height, std::vector<i64>(ss_width, 0));

#pragma omp parallel num_threads(T)
    {
        std::mt19937 rng(omp_get_thread_num() + 1234);
        std::uniform_real_distribution<f64> dist_x(this->x1, this->x2);
        std::uniform_real_distribution<f64> dist_y(this->y1, this->y2);

        std::vector<std::vector<i64>> local_hist(ss_height, std::vector<i64>(ss_width, 0));

#pragma omp for schedule(dynamic)
        for (i64 s = 0; s < samples; ++s)
        {
            f64 cx = dist_x(rng);
            f64 cy = dist_y(rng);
            CF64 c(cx, cy);
            CF64 z(0, 0);
            std::vector<std::pair<f64, f64>> orbit;
            bool escaped = false;

            int escape_iter = -1;
            for (int i = 0; i < iters; ++i)
            {
                z = z * z + c;
                orbit.emplace_back(z.real(), z.imag());
                if (std::abs(z) > 2.0)
                {
                    escaped = true;
                    escape_iter = i;
                    break;
                }
            }

            const int min_escape = 100;
            const int max_escape = iters - 10;
            if (escaped && escape_iter >= min_escape && escape_iter < max_escape)
            {
                int step = 2;
                for (size_t j = 0; j < orbit.size(); j += step)
                {
                    const auto &pt = orbit[j];
                    f64 fx = (pt.first - this->x1) / (this->x2 - this->x1);
                    f64 fy = (this->y2 - pt.second) / (this->y2 - this->y1);
                    // Map to supersampled grid
                    int x = static_cast<int>(fx * (ss_width - 1));
                    int y = static_cast<int>(fy * (ss_height - 1));
                    if (x >= 0 && x < ss_width && y >= 0 && y < ss_height)
                        local_hist[y][x]++;
                }
            }
        }

#pragma omp critical
        {
            for (int y = 0; y < ss_height; ++y)
                for (int x = 0; x < ss_width; ++x)
                    hist[y][x] += local_hist[y][x];
        }
    }

    // Downsample to target resolution with box filter
    std::vector<std::vector<f64>> downsampled(this->height, std::vector<f64>(this->width, 0.0));
    for (int y = 0; y < this->height; ++y)
    {
        for (int x = 0; x < this->width; ++x)
        {
            i64 sum = 0;
            for (int dy = 0; dy < supersample; ++dy)
                for (int dx = 0; dx < supersample; ++dx)
                    sum += hist[y * supersample + dy][x * supersample + dx];
            downsampled[y][x] = static_cast<f64>(sum) / (supersample * supersample);
        }
    }

    // Find max for normalization
    f64 max_val = 1e-8;
    for (int y = 0; y < this->height; ++y)
        for (int x = 0; x < this->width; ++x)
            if (downsampled[y][x] > max_val)
                max_val = downsampled[y][x];

    // Map histogram to image
    for (int y = 0; y < this->height; ++y)
    {
        for (int x = 0; x < this->width; ++x)
        {
            f64 t = downsampled[y][x] / max_val;
            color_t color = colormap ? colormap(t) : color_t{static_cast<channel>(t * 255), static_cast<channel>(t * 255), static_cast<channel>(t * 255)};
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
