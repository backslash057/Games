from PIL import Image


gif = Image.open("Garou_05_1.gif")

for i in range(gif.n_frames):
	gif.seek(i)

	gif.save(f"stages/garou_5/{i}.png")