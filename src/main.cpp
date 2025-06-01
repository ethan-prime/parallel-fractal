#include "render.h"
#include "fractal.h"
#include <cstdlib>
#include <iostream>
#include <chrono>

#define RES_4K 3840, 2160
#define RES_HD 1920, 1080

int main(int argc, char *argv[])
{
    // Defaults
    int width = 3840;
    int height = 2160;
    int x1 = -2, y1 = -2, x2 = 2, y2 = 2;
    double c_real = -0.8, c_imag = 0.156;
    int max_iters = 500;

    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " num_threads [width height x1 y1 x2 y2 c_real c_imag [max_iters]]\n";
        std::cout << "Example: " << argv[0] << " 8 3840 2160 -2 -2 2 2 -0.8 0.156 500\n";
        return 1;
    }

    int num_threads = std::atoi(argv[1]);
    std::cout << "Num threads: " << num_threads << std::endl;

    // Optional args: width height x1 y1 x2 y2 c_real c_imag [max_iters]
    if (argc > 2)
    {
        if (argc < 10)
        {
            std::cout << "Usage: " << argv[0] << " num_threads [width height x1 y1 x2 y2 c_real c_imag [max_iters]]\n";
            std::cout << "Using defaults: 3840 2160 -2 -2 2 2 -0.8 0.156 500\n";
        }
        else
        {
            width = std::atoi(argv[2]);
            height = std::atoi(argv[3]);
            x1 = std::atoi(argv[4]);
            y1 = std::atoi(argv[5]);
            x2 = std::atoi(argv[6]);
            y2 = std::atoi(argv[7]);
            c_real = std::atof(argv[8]);
            c_imag = std::atof(argv[9]);
            if (argc > 10)
            {
                max_iters = std::atoi(argv[10]);
            }
        }
    }

    auto start = std::chrono::high_resolution_clock::now();

    Image img(width, height, x1, y1, x2, y2);
    CF64 c(c_real, c_imag);
    img.Render(julia, c, max_iters, num_threads, wikipedia_colormap);
    img.Save("res.jpeg");

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Execution time: " << elapsed.count() << " seconds\n";

    return 0;
}