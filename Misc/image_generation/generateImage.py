from PIL import Image, ImageDraw
import io

IMAGE_WIDTH = 60
IMAGE_HEIGHT = 24

file = open("output.h", "w")

img = Image.open('usb_small.png') #.convert('1')
#img = img.convert('L')
#img = img.resize((IMAGE_WIDTH,IMAGE_HEIGHT))
#img.save('usb_small.png')

pixelData = img.getdata()
outputArray = []

for y in xrange(0, IMAGE_HEIGHT / 8):
    for x in xrange(0, IMAGE_WIDTH):
        currentByte = 0
        for bit in xrange(0, 8):
            transparency = pixelData[x + (y * 8 + bit) * IMAGE_WIDTH][3]
            red = pixelData[x + (y * 8 + bit) * IMAGE_WIDTH][0]
            currentByte |= (1 if (0 != transparency) and (255 != red) else 0) << bit
        outputArray.append(currentByte)

file.write("        { ")
byteCount = 0
for byte in outputArray:
    file.write(("0x%02X" % byte) + ", ")
    byteCount += 1
    if (16 == byteCount):
        byteCount = 0
        file.write("\n        ")
file.write(" },\n")
file.close()

print pixelData[483]