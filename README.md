# parallel-fractal

A fast, parallelized C++ renderer for Julia and Mandelbrot fractals, using OpenMP for multi-threading and supporting high-quality anti-aliased output with customizable colormaps. Output images are saved as JPEGs.

## Features

- Multi-threaded rendering with OpenMP
- Anti-aliasing via multi-sample per pixel
- Customizable colormaps (Wikipedia-style, grayscale, etc.)
- High-resolution output (4K and beyond)

## Usage

Build the project:

```sh
make
```

Run the fractal renderer:

```sh
./fractal num_threads [width height x1 y1 x2 y2 c_real c_imag [max_iters gamma]]
```

### Arguments

- `num_threads` — Number of threads to use (e.g., 8)
- `width height` — Output image resolution (e.g., 3840 2160)
- `x1 y1 x2 y2` — Complex plane region to render (e.g., -2 -2 2 2)
- `c_real c_imag` — Julia set constant (e.g., -0.8 0.156)
- `max_iters` — (Optional) Maximum iterations per pixel (default: 500)
- `gamma` — (Optional) Gamma correction for colormap (default: 0.5)

Example:

```sh
./fractal 8 3840 2160 -2 -2 2 2 -0.8 0.156 1000 0.4
```

This will render a 4K Julia set using 8 threads, with 1000 max iterations and gamma 0.4.

## Viewing the Output

If running under WSL, use the provided script to open the image in Windows:

```sh
./show
```

## Requirements

- C++17 compiler
- OpenMP
- libjpeg

## About

This project is for exploring and rendering fractal images efficiently using modern C++ and parallelism. I designed it for my CS358 Final Project at Northwestern University with Professor Joe Hummel.

---
