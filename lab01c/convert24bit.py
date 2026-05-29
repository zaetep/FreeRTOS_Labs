from PIL import Image

img = Image.open("logoresized.bmp")

# Convert to 24-bit RGB
img = img.convert("RGB")
img.save("output_24bit.bmp")