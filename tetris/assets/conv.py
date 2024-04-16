from PIL import Image

image = Image.open("main.png")
image = image.convert("RGBA")

data = image.getdata()
new = []

for item in data:
	if item[0]>200 and item[1]>200 and item[2]>200:
		item = (255, 255, 255, 0)
	new.append(item)


image.putdata(new)




image.save("output.png")