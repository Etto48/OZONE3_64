#!/usr/bin/python3

from PIL import Image
import numpy as np


file_name=input("Filename:")
I=np.asarray(Image.open(file_name))

with open(file_name+".txt","w") as f:
    for row in I:
        for pixel in row:
            if(len(pixel)==3):
                f.write(f"0xff{pixel[0]:02X}{pixel[1]:02X}{pixel[2]:02X}")
            else:
                f.write(f"0x{pixel[3]:02X}{pixel[0]:02X}{pixel[1]:02X}{pixel[2]:02X}")
            f.write(",")
        f.write("\n")