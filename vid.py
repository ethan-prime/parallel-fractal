import numpy as np
import subprocess
import os
from tqdm import tqdm

# Parameters
width = 2000
height = 2000
x1, y1, x2, y2 = -1.5, -1.5, 1.5, 1.5
num_threads = 16
max_iters = 500
gamma = 0.5
frames = 1200  # 60 fps * 20 seconds for super smooth animation
output_dir = "frames"
fractal_bin = "./fractal"

# Make output directory
os.makedirs(output_dir, exist_ok=True)

# Render frames
for i, a in enumerate(tqdm(np.linspace(0, 2 * np.pi, frames, endpoint=False), total=frames, desc="Rendering frames")):
    c_real = 0.7885 * np.cos(a)
    c_imag = 0.7885 * np.sin(a)
    outname = os.path.join(output_dir, f"frame_{i:04d}.jpeg")
    cmd = [
        fractal_bin,
        str(num_threads),
        "julia",
        str(width), str(height),
        str(x1), str(y1), str(x2), str(y2),
        str(c_real), str(c_imag),
        str(max_iters), str(gamma)
    ]
    # Optionally, comment out the next line to reduce console spam:
    # print(f"Rendering frame {i+1}/{frames}: a={a:.3f}, c=({c_real:.5f}, {c_imag:.5f})")
    subprocess.run(cmd)
    os.rename("res.jpeg", outname)

print("All frames rendered.")
print("To make a video, run:")
print(f"ffmpeg -framerate 60 -i frames/frame_%04d.jpeg -c:v libx264 -pix_fmt yuv420p julia_anim.mp4")
print("To make a GIF (from the mp4), run:")
print("ffmpeg -i julia_anim.mp4 -vf 'fps=15,scale=600:-1:flags=lanczos' -gifflags -transdiff -y julia_anim.gif")
