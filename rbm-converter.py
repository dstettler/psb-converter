from PIL import Image
import numpy as np
import argparse

parser = argparse.ArgumentParser(prog='RBM Converter',description='Converts images into raw RGB(A) arrays')
parser.add_argument('filename')
parser.add_argument('-o', '--output', required=False, help='File to output rbm contents to')

args = parser.parse_args()

filepath: str = args.filename
split_filepath: list[str] = filepath.split('.')
filename: str = ''

# Remove the file extension
for i, text in enumerate(split_filepath):
    if i < len(split_filepath) - 1:
        filename += text
    else:
        break

image = Image.open(filepath)
image = np.asarray(image)

baselist = []

for i in range(image.shape[0]):
    for j in range(image.shape[1]):
        baselist.append(image[i][j])
                        

rbm_name = f'{filename}.rbm'
if args.output != None:
    rbm_name = args.output

with open(rbm_name, 'w') as f:
    f.write(f"{image.shape[0]},{image.shape[1]}\n")
    for line in baselist:
        blist = list(line.tobytes())
        extra = ''
        if len(blist) > 3:
            extra = "," + blist[3]
        f.write(f"{blist[0]},{blist[1]},{blist[2]}{extra}\n")
