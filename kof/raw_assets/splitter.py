from PIL import Image
import numpy as np


left_pad = 14
top_pad = 1
size_x = 752
size_y = 245

margin = 4


image = Image.open("Town Stage.png")
image_np = np.array(image)

for i in range(8):
	for j in range(1):
		kernel = image_np[i*size_y+top_pad+i*margin:(i+1)*size_y+top_pad+i*margin, j*size_x+left_pad+j*margin:(j+1)*size_x+left_pad+j*margin]

		output = Image.fromarray(kernel)

		output.save(f"stages/town/{i*4+j}.png")