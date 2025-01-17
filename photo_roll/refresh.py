import os
import shutil

home_path = os.path.dirname(os.path.realpath(__file__))

def crawl_dir(path):
	if os.path.exists(path) == False: raise Exception(f"path \'{path}\' is either not an existing path or requires higher permissions")

	for item in os.listdir(path):
		item_path = os.path.join(path, item)

		if not os.path.isfile(item_path):
			if item == "results":
				for image in os.listdir(item_path):
					if not os.path.exists(os.path.join(home_path, image)):
						shutil.copy(os.path.join(item_path, image), home_path)
						print("added:", os.path.join(item_path, image))
				break
			elif item != "include" and item != "lib" and item != "src":
				crawl_dir(item_path)

crawl_dir("..")