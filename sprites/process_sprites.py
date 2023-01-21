# CODE TO CONVERT BMP SPRITES TO CBT (CENGINE BLOCK TEXTURE)
# SUPPORTS EXPORTING 1 BIT AND 8 BIT CBT FILES
# NEEDS TO BE IN BASE DIRECTORY WHERE RAW AND GAME SPRITES ARE STORED!

import numpy as np
from PIL import Image
from pathlib import Path

# Convert all sprites to raw numpy arrays, and export back out to cbt format
raw_sprites_path = Path('./raw_sprites')
asset_sprites_path = Path('./game_sprites')

# Every pixel is 1 byte in the numpy array for 8 bit or 1 bit bmps.
for p in raw_sprites_path.glob('**/*.bmp'):
    im = Image.open(p)
    width, height = im.size
    im_arr = np.array(im, dtype=np.uint8)

    # Check if file is 1 bit.
    # If so, pack bits!
    if np.all((im_arr == 1)|(im_arr == 0)):
        im_arr = np.packbits(im_arr, axis=-1)

    # Prepare image to be opened.
    f = open(asset_sprites_path / f"{p.stem}.cbt", "wb")

    # Check if alpha channel exists to export as cbta
    potential_alpha = raw_sprites_path / f"{p.stem}_alpha.bmp"

    f.write(np.uint8(potential_alpha.is_file())) # Write wether alpha enabled
    f.write(np.uint8(width)) # Width
    f.write(np.uint8(height)) # Height

    if potential_alpha.is_file():
        alpha = Image.open(potential_alpha)

        # Pack bits since alpha should always be 1 bit.
        alpha_arr = np.packbits(np.array(alpha, dtype=np.int8), axis=-1)

        # Write image and alpha at once.
        f.write(im_arr.tobytes())
        f.write(alpha_arr.tobytes())

    # Write cbt without transparency.
    else:
        f.write(im_arr.tobytes())

    f.close()
