import numpy as np
from PIL import Image

# Définir les dimensions de l'image
BATTERY_HEIGHT = 20
BATTERY_WIDTH = 40

# Définir les données de l'image monochrome
BATTERY_FULL = [
    0x1f, 0xff, 0xff, 0xff, 0xc0,
    0x3f, 0xff, 0xff, 0xff, 0xe0,
    0x7f, 0xff, 0xff, 0xff, 0xf0,
    0x78, 0x00, 0x00, 0x00, 0x70,
    0x70, 0x00, 0x00, 0x00, 0x30,
    0x70, 0x00, 0x00, 0x00, 0x30,
    0x70, 0x00, 0x00, 0x00, 0x3e,
    0x70, 0x00, 0x00, 0x00, 0x3f,
    0x70, 0x00, 0x00, 0x00, 0x31,
    0x70, 0x00, 0x00, 0x00, 0x31,
    0x70, 0x00, 0x00, 0x00, 0x31,
    0x70, 0x00, 0x00, 0x00, 0x31,
    0x70, 0x00, 0x00, 0x00, 0x3f,
    0x70, 0x00, 0x00, 0x00, 0x3e,
    0x70, 0x00, 0x00, 0x00, 0x30,
    0x70, 0x00, 0x00, 0x00, 0x30,
    0x78, 0x00, 0x00, 0x00, 0x70,
    0x7f, 0xff, 0xff, 0xff, 0xf0,
    0x3f, 0xff, 0xff, 0xff, 0xe0,
    0x1f, 0xff, 0xff, 0xff, 0xc0
]

# Créer une image à partir des données
image_array = np.zeros((BATTERY_HEIGHT, BATTERY_WIDTH), dtype=np.uint8)

# Remplir le tableau avec les données de l'array
for i in range(BATTERY_HEIGHT):
    for j in range(BATTERY_WIDTH):
        byte_index = i * 5 + j // 8
        bit_index = 7 - (j % 8)
        if (BATTERY_FULL[byte_index] & (1 << bit_index)):
            image_array[i, j] = 255
        else:
            image_array[i, j] = 0

# Convertir le tableau en image
img = Image.fromarray(image_array, 'L')  # 'L' pour monochrome

# Enregistrer l'image en BMP
img.save('battery_0.bmp')

print("L'image a été enregistrée sous le nom 'battery_image.bmp'")