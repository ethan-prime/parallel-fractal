#include "render.h"
#include "fractal.h"
#include "colormap.h"
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <string>

#define RES_4K 3840, 2160
#define RES_HD 1920, 1080

f64 GAMMA = 0.5;

int main(int argc, char *argv[])
{
    // Defaults
    int width = 2000;
    int height = 2000;
    double x1 = -2, y1 = -2, x2 = 2, y2 = 2;
    double c_real = -0.8, c_imag = 0.156;
    int max_iters = 500;
    auto fractal_func = julia; // default
    std::string fractal_type = "julia";
    std::string colormap_name = "grayscale";
    colormap_t colormap = nullptr;

    if (argc < 3)
    {
        std::cout << "Usage: " << argv[0] << " num_threads (julia|mandelbrot|buddhabrot) [colormap] [width height x1 y1 x2 y2 c_real c_imag [max_iters gamma]]\n";
        std::cout << "Colormaps: grayscale, wikipedia, buddha\n";
        std::cout << "Example: " << argv[0] << " 8 julia wikipedia 2000 2000 -2 -2 2 2 -0.8 0.156 500 0.5\n";
        return 1;
    }

    int num_threads = std::atoi(argv[1]);
    fractal_type = argv[2];
    int arg_offset = 3;

    // Parse colormap argument if present and not a number
    if (argc > 3 && !(std::isdigit(argv[3][0]) || argv[3][0] == '-'))
    {
        colormap_name = argv[3];
        arg_offset = 4;
    }

    // Select colormap
    if (colormap_name == "wikipedia")
        colormap = wikipedia_colormap;
    else if (colormap_name == "buddha")
        colormap = buddha_bright_colormap;
    else if (colormap_name == "buddhablue")
        colormap = buddha_bright_blue;
    else if (colormap_name == "buddhared")
        colormap = buddha_bright_red;
    else if (colormap_name == "buddhagold")
        colormap = buddha_bright_gold;
    else if (colormap_name == "buddhasky")
        colormap = buddha_bright_sky;
    else if (colormap_name == "blacksky")
        colormap = black_to_sky;
    else if (colormap_name == "burningshipfire")
        colormap = burning_ship_fire_colormap;
    else if (colormap_name == "grayscale")
        colormap = grayscale_colormap;
    else // default: grayscale
        colormap = grayscale_colormap;

    if (fractal_type == "mandelbrot")
        fractal_func = mandelbrot;
    else if (fractal_type == "julia")
        fractal_func = julia;
    else if (fractal_type == "burningship")
        fractal_func = burningship;
    else if (fractal_type == "buddhabrot")
        fractal_func = nullptr;
    else if (fractal_type == "buddhaship")
        fractal_func = nullptr;
    else
    {
        std::cout << "Unknown fractal type: " << fractal_type << ". Use 'julia', 'mandelbrot', 'burningship', or 'buddhabrot'.\n";
        return 1;
    }
    std::cout << "Num threads: " << num_threads << std::endl;
    std::cout << "Fractal: " << fractal_type << std::endl;
    std::cout << "Colormap: " << colormap_name << std::endl;

    // Optional args: width height x1 y1 x2 y2 c_real c_imag [max_iters gamma]
    if (argc > arg_offset)
    {
        if (argc < arg_offset + 8)
        {
            std::cout << "Usage: " << argv[0] << " num_threads (julia|mandelbrot|buddhabrot) [colormap] [width height x1 y1 x2 y2 c_real c_imag [max_iters gamma]]\n";
            std::cout << "Using defaults: 2000 2000 -2 -2 2 2 -0.8 0.156 500 0.5\n";
        }
        else
        {
            width = std::atoi(argv[arg_offset]);
            height = std::atoi(argv[arg_offset + 1]);
            x1 = std::atof(argv[arg_offset + 2]);
            y1 = std::atof(argv[arg_offset + 3]);
            x2 = std::atof(argv[arg_offset + 4]);
            y2 = std::atof(argv[arg_offset + 5]);
            c_real = std::atof(argv[arg_offset + 6]);
            c_imag = std::atof(argv[arg_offset + 7]);
            if (argc > arg_offset + 8)
            {
                max_iters = std::atoi(argv[arg_offset + 8]);
                if (argc > arg_offset + 9)
                    GAMMA = std::atof(argv[arg_offset + 9]);
            }
        }
    }

    auto start = std::chrono::high_resolution_clock::now();

    Image img(width, height, x1, y1, x2, y2);
    CF64 c(c_real, c_imag);

    if (fractal_type == "buddhabrot")
        img.RenderBuddhabrot(max_iters, num_threads, 500000000, colormap);
    else if (fractal_type == "buddhaship")
        img.RenderBurningShipBuddhabrot(max_iters, num_threads, 500000000, colormap);
    else
        img.Render(fractal_func, c, max_iters, num_threads, colormap);

    img.Save("res.jpeg");

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Execution time: " << elapsed.count() << " seconds\n";

    return 0;
}