from PIL import Image
import numpy as np
import argparse

filename = 'image'
path = f'{filename}.png'
image = Image.open(path)
image = np.asarray(image)

baselist = []

for i in range(image.shape[0]):
    for j in range(image.shape[1]):
        baselist.append(image[i][j])
                        
with open(f'{filename}.rbm', 'w') as f:
    f.write(f"{image.shape[0]},{image.shape[1]}\n")
    for line in baselist:
        blist = list(line.tobytes())
        extra = ''
        if len(blist) > 3:
            extra = "," + blist[3]
        f.write(f"{blist[0]},{blist[1]},{blist[2]}{extra}\n")
