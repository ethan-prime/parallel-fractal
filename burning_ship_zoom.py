import numpy as np
import subprocess
import os
from tqdm import tqdm

# Parameters
width = 1290
height = 720
num_threads = 16
max_iters = 100
gamma = 0.5
frames = 240  # 8 seconds at 30fps
output_dir = "frames_burningship_zoom"
fractal_bin = "./fractal"
colormap = "burningshipfire"

# Zoom target: Burning Ship antennas (well-known deep zoom)
center_x, center_y = -1.7625, -0.025  # Antenna region
start_scale = 0.08  # Initial half-width of view
end_scale = 0.0005  # Final half-width of view

os.makedirs(output_dir, exist_ok=True)

for i, t in enumerate(tqdm(np.linspace(0, 1, frames), total=frames, desc="Rendering Burning Ship zoom")):
    # Exponential zoom for smoothness
    scale = start_scale * (end_scale / start_scale) ** t
    x1 = center_x - scale
    x2 = center_x + scale
    # Maintain aspect ratio
    aspect = height / width
    y_half = scale * aspect
    y1 = center_y - y_half
    y2 = center_y + y_half

    outname = os.path.join(output_dir, f"frame_{i:04d}.jpeg")
    cmd = [
        fractal_bin,
        str(num_threads),
        "burningship",
        colormap,
        str(width), str(height),
        str(x1), str(y1), str(x2), str(y2),
        "0", "0",
        str(max_iters), str(gamma)
    ]
    subprocess.run(cmd)
    os.rename("res.jpeg", outname)

print("All frames rendered.")
print("To make a video, run:")
print(f"ffmpeg -framerate 30 -i {output_dir}/frame_%04d.jpeg -c:v libx264 -pix_fmt yuv420p burning_ship_zoom.mp4")
