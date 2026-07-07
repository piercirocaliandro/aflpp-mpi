import struct
import os

FUZZ_DIR = "/home/cr0/libexif_example/output/"  # output directory for fuzzing
BITMAP_FILE = "fuzz_bitmap"
bitmap_paths = []

for root, _, files in os.walk(FUZZ_DIR):
    if BITMAP_FILE in files:
        bitmap_paths.append(root+"/"+BITMAP_FILE)


bitmaps = []
for bitmap in bitmap_paths:
    with open(bitmap, "rb") as fb:
        content = fb.read()
        x = list(struct.unpack("<"+str(len(content))+'B', bytearray(content)))
        print(len(x))
        
        # Bytemap normalization to bitmap
        for i in range(0, len(x)):
            if x[i] == 255:
                x[i] = 0
            else:
                x[i] = 1
        bitmaps.append(x)

for b in bitmaps:
    ones = 0
    for i in b:
        if i == 1:
            ones += 1

    print(ones/len(b))

final_bitmap = bitmaps[0]

for i in range(1, len(bitmaps)):
    for j in range(0, len(final_bitmap)):
        final_bitmap[j] = final_bitmap[j] | bitmaps[i][j]

ones = 0
for i in final_bitmap:
    if i == 1:
        ones += 1

print("\n{}\n".format(ones/len(final_bitmap)))
