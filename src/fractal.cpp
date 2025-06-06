#include "render.h"

/*def julia(z: complex, c: complex, max_iterations: int) -> float:
    for i in range(1, max_iterations):
        z = z**2+c
        if abs(z) > 2:
            return i/max_iterations
    return 1
*/

#define R 2

f64 julia(CF64 z, CF64 c, i32 iters)
{
    for (int i = 0; i < iters; i++)
    {
        z = z * z + c;
        if (std::abs(z) > R)
        {
            // Debug: print escape info
            // std::cout << "escaped at i=" << i << ", |z|=" << std::abs(z) << std::endl;
            return static_cast<f64>(i) / iters;
        }
    }
    return 1.0;
}

f64 mandelbrot(CF64 c, CF64 _, i32 iters)
{
    CF64 z(0, 0);
    for (int i = 0; i < iters; i++)
    {
        z = z * z + c;
        if (std::abs(z) > R)
        {
            return static_cast<f64>(i) / iters;
        }
    }
    return 1.0;
}

f64 burningship(CF64 c, CF64 /*unused*/, i32 iters)
{
    CF64 z(0, 0);
    for (int i = 0; i < iters; i++)
    {
        // Burning Ship: z = (|Re(z)| + i|Im(z)|)^2 + c
        double zr = std::abs(z.real());
        double zi = std::abs(z.imag());
        z = CF64(zr, zi) * CF64(zr, zi) + c;
        if (std::abs(z) > R)
            return static_cast<f64>(i) / iters;
    }
    return 1.0;
}