from PIL import Image, ImageDraw
import io

IMAGE_WIDTH = 16
IMAGE_HEIGHT = 16
N_RESIZE = 64

file = open("symbol.h", "w")

image = Image.new('1', (16 * N_RESIZE, 16 * N_RESIZE), 'white')
draw = ImageDraw.Draw(image)
#draw.ellipse((0 * N_RESIZE, 1 * N_RESIZE, 16 * N_RESIZE, 14 * N_RESIZE), fill ='black', outline ='black')
#draw.ellipse((2 * N_RESIZE, 3 * N_RESIZE, 14 * N_RESIZE, 12 * N_RESIZE), fill ='white', outline ='white')
draw.polygon([(0 * N_RESIZE, 1 * N_RESIZE), (16 * N_RESIZE, 7.5 * N_RESIZE), (0 * N_RESIZE, 14 * N_RESIZE)], fill ='black', outline ='black')

image = image.resize((IMAGE_WIDTH,IMAGE_HEIGHT))
pixelData = image.getdata()
outputArray = []

for y in xrange(0, IMAGE_HEIGHT / 8):
    for x in xrange(0, IMAGE_WIDTH):
        currentByte = 0
        for bit in xrange(0, 8):
            currentByte |= (1 if (0 == pixelData[x + (y*8+bit)*IMAGE_WIDTH]) else 0) << bit
        outputArray.append(currentByte)

print outputArray

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

image.save('test.png')