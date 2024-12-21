from PIL import Image
import os

for i in range(50):
	try:
		gif = Image.open(f"animated-gifs-of-fighting-game-backgrounds-{i}.gif")

		print(f"Background_{i}: {gif.n_frames}")
		for j in range(gif.n_frames):
			gif.seek(j)
			gif.save(f"stages/background_{i}/{j}.png")
	except Exception as e: print(e)
