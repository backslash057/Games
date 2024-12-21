import os
import os.path

path = os.getcwd()
files = os.listdir()

# print(os.path.__dir__())
for file in files:
	final_path = os.path.join(path, file)
	
	if os.path.isfile(final_path) and file.endswith(".png"):
		name = file[:-4]
		print(name)
		# try:
		# 	os.mkdir(os.path.join(name, "portraits"))
		# except: pass
