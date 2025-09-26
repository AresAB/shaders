import sys
import os

def main():
    scr_size = [2880 / 2 - 2, 1800 / 2 - 10]

    image_info = " ".join(sys.argv).split(",")

    image_dimensions = image_info[1].split(" x ")
    image_dimensions[0] = int(image_dimensions[0])
    image_dimensions[1] = int(image_dimensions[1])

    screen_shot_dimensions = image_dimensions.copy()

    while image_dimensions[0] < scr_size[0] and image_dimensions[1] < scr_size[1]:
        image_dimensions[0] *= 2
        image_dimensions[1] *= 2

    while image_dimensions[0] > scr_size[0] or image_dimensions[1] > scr_size[1]:
        image_dimensions[0] /= 2
        image_dimensions[1] /= 2

    image_color = image_info[2].split()[1]

    image_dir = "results/" + image_info[-1].split("/")[-1]
    counter = "_" + str(len(os.listdir("./results")))

    x_heavy = int(image_dimensions[0] > image_dimensions[1])
    image_dimensions[0] += image_dimensions[0] * (1 - x_heavy)
    image_dimensions[1] += image_dimensions[1] * x_heavy

    print(image_dimensions[0], image_dimensions[1], x_heavy, screen_shot_dimensions[0], screen_shot_dimensions[1], image_color, image_dir + counter + ".png")

main()