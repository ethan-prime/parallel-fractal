import numpy as np
import subprocess
import os

# Parameters
width = 1000
height = 1000
num_threads = 16
max_iters = 500
gamma = 0.6
frames = 1200  # 60 fps * 20 seconds
output_dir = "frames_mandelbrot_zoom"
fractal_bin = "./fractal"

# Zoom parameters
center_x, center_y = -0.743643887037158704752191506114774, 0.131825904205311970493132056385139  # Deep zoom location
start_scale = 2.0  # Initial half-width/height of view
end_scale = 0.000005  # Final half-width/height of view

os.makedirs(output_dir, exist_ok=True)

for i, t in enumerate(np.linspace(0, 1, frames)):
    print(f"frame {i+1}/{frames}")
    # Exponential zoom for smoothness
    scale = start_scale * (end_scale / start_scale) ** t
    x1 = center_x - scale
    x2 = center_x + scale
    y1 = center_y - scale
    y2 = center_y + scale

    outname = os.path.join(output_dir, f"frame_{i:04d}.jpeg")
    cmd = [
        fractal_bin,
        str(num_threads),
        "mandelbrot",
        str(width), str(height),
        str(x1), str(y1), str(x2), str(y2),
        "0", "0",  # c_real, c_imag (ignored for mandelbrot)
        str(max_iters), str(gamma)
    ]
    subprocess.run(cmd)
    os.rename("res.jpeg", outname)

print("All frames rendered.")
print("To make a video, run:")
print(f"ffmpeg -framerate 60 -i {output_dir}/frame_%04d.jpeg -c:v libx264 -pix_fmt yuv420p mandelbrot_zoom.mp4")
print("To make a GIF (from the mp4), run:")
print("ffmpeg -i mandelbrot_zoom.mp4 -vf 'fps=15,scale=600:-1:flags=lanczos' -gifflags -transdiff -y mandelbrot_zoom.gif")
