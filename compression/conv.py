import numpy as np
import os
os.environ['OPENCV_IO_ENABLE_OPENEXR'] = '1'
import cv2
from myqoi import *

os.environ['OPENCV_IO_ENABLE_OPENEXR'] = '1'


def exr_to_png(file):
    im = cv2.imread(file, cv2.IMREAD_ANYCOLOR | cv2.IMREAD_ANYDEPTH)
    im = im*65535
    im[im > 65535] = 65535
    im = np.uint16(im)
    cv2.imwrite(file.split(".")[0] + ".png", im)


if __name__ == "__main__":
    file = argv[1]
    filname, suffix = file.split(".")

    if suffix == "qoi":
        qoi_to_png(file, filname + ".png")
    elif suffix == "jpg":
        png_to_qoi(file, filname + ".qoi")
    elif suffix == "png":
        png_to_qoi(file, filname + ".qoi")
    elif suffix == "exr":
        exr_to_png(file)
        png_to_qoi(filname + ".png", filname + ".qoi")