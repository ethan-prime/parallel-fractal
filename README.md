# parallel-fractal

A fast, parallelized C++ renderer for Julia, Mandelbrot, Burning Ship, Buddhabrot, and Burning Ship Buddhabrot fractals, using OpenMP for multi-threading and supporting high-quality anti-aliased output with customizable colormaps. Output images are saved as JPEGs.

## Features

- Multi-threaded rendering with OpenMP
- Anti-aliasing via multi-sample per pixel and supersampling
- Customizable colormaps (Wikipedia-style, grayscale, fire, gradients, etc.)
- High-resolution output (4K and beyond)
- Supports Julia, Mandelbrot, Burning Ship, Buddhabrot, and Burning Ship Buddhabrot
- Easy parameterization for zoom, region, and fractal constants

## Usage

Build the project:

```sh
make
```

Run the fractal renderer:

```sh
./fractal num_threads (julia|mandelbrot|burningship|buddhabrot|buddhaship) [colormap] [width height x1 y1 x2 y2 c_real c_imag [max_iters gamma]]
```

### Arguments

- `num_threads` — Number of threads to use (e.g., 8, 16)
- `julia|mandelbrot|burningship|buddhabrot|buddhaship` — Which fractal to render
- `colormap` — (Optional) Colormap to use (see below)
- `width height` — Output image resolution (e.g., 3840 2160)
- `x1 y1 x2 y2` — Complex plane region to render (e.g., -2 -2 2 2)
- `c_real c_imag` — Julia set constant (e.g., -0.8 0.156). For Mandelbrot/Burning Ship/Buddhabrot, these are ignored but must be present.
- `max_iters` — (Optional) Maximum iterations per pixel (default: 500)
- `gamma` — (Optional) Gamma correction for colormap (default: 0.5)

### Example Commands

**Julia set:**
```sh
./fractal 8 julia wikipedia 3840 2160 -2 -2 2 2 -0.8 0.156 1000 0.4
```

**Mandelbrot set:**
```sh
./fractal 8 mandelbrot wikipedia 3840 2160 -2 -2 2 2 0 0 1000 0.4
```

**Burning Ship:**
```sh
./fractal 16 burningship burningshipfire 2560 1440 -1.8388889 -0.09 -1.6611111 0.01 0 0 1000 0.5
```

**Buddhabrot:**
```sh
./fractal 16 buddhabrot buddha 2000 2000 -2 -2 2 2 0 0 1000 0.4
```

**Burning Ship Buddhabrot:**
```sh
./fractal 16 buddhaship burningshipfire 2000 2000 -2 -2 2 2 0 0 1000 0.4
```

### Colormaps

- `grayscale` (default)
- `wikipedia` (smooth blue-yellow-red)
- `buddha` (bright white Buddhabrot)
- `buddhablue`, `buddhared`, `buddhagold`, `buddhasky` (colored Buddhabrot)
- `blacksky` (black to sky blue gradient)
- `burningshipfire` (black ship, fire/orange/yellow/white background)
- You can easily add your own gradients in `colormap.h`

## Viewing the Output

If running under WSL, use the provided script to open the image in Windows:

```sh
./show
```

## Requirements

- C++17 compiler
- OpenMP
- libjpeg
- Python 3 (for animation scripts, optional)
- ffmpeg (for video creation, optional)

## Animation Scripts

Python scripts are provided for generating zoom animations:

- `vid.py` — Julia set animation
- `burning_ship_zoom.py` — Burning Ship zoom animation

## About

This project is for exploring and rendering fractal images efficiently using modern C++ and parallelism. I designed it for my CS358 Final Project at Northwestern University with Professor Joe Hummel.

---