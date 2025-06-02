#include "render.h"
#include "fractal.h"
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
    int width = 3840;
    int height = 2160;
    double x1 = -2, y1 = -2, x2 = 2, y2 = 2;
    double c_real = -0.8, c_imag = 0.156;
    int max_iters = 500;
    auto fractal_func = julia; // default
    std::string fractal_type = "julia";

    if (argc < 3)
    {
        std::cout << "Usage: " << argv[0] << " num_threads (julia|mandelbrot) [width height x1 y1 x2 y2 c_real c_imag [max_iters gamma]]\n";
        std::cout << "Example: " << argv[0] << " 8 julia 3840 2160 -2 -2 2 2 -0.8 0.156 500 0.5\n";
        return 1;
    }

    int num_threads = std::atoi(argv[1]);
    fractal_type = argv[2];
    if (fractal_type == "mandelbrot")
        fractal_func = mandelbrot;
    else if (fractal_type == "julia")
        fractal_func = julia;
    else
    {
        std::cout << "Unknown fractal type: " << fractal_type << ". Use 'julia' or 'mandelbrot'.\n";
        return 1;
    }
    std::cout << "Num threads: " << num_threads << std::endl;
    std::cout << "Fractal: " << fractal_type << std::endl;

    // Optional args: width height x1 y1 x2 y2 c_real c_imag [max_iters gamma]
    if (argc > 3)
    {
        if (argc < 11)
        {
            std::cout << "Usage: " << argv[0] << " num_threads (julia|mandelbrot) [width height x1 y1 x2 y2 c_real c_imag [max_iters gamma]]\n";
            std::cout << "Using defaults: 3840 2160 -2 -2 2 2 -0.8 0.156 500 0.5\n";
        }
        else
        {
            width = std::atoi(argv[3]);
            height = std::atoi(argv[4]);
            x1 = std::atof(argv[5]);
            y1 = std::atof(argv[6]);
            x2 = std::atof(argv[7]);
            y2 = std::atof(argv[8]);
            c_real = std::atof(argv[9]);
            c_imag = std::atof(argv[10]);
            if (argc > 11)
            {
                max_iters = std::atoi(argv[11]);
                if (argc > 12)
                    GAMMA = std::atof(argv[12]);
            }
        }
    }

    auto start = std::chrono::high_resolution_clock::now();

    Image img(width, height, x1, y1, x2, y2);
    CF64 c(c_real, c_imag);
    img.Render(fractal_func, c, max_iters, num_threads, wikipedia_colormap);
    img.Save("res.jpeg");

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Execution time: " << elapsed.count() << " seconds\n";

    return 0;
}