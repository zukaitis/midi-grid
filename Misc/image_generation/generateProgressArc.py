from PIL import Image, ImageDraw
import io

IMAGE_WIDTH = 38
IMAGE_HEIGHT = 32
N_RESIZE = 64
NUMBER_OF_POSITIONS = 51 # [0;100]

file = open("progressArc.h", "w")
file.write("#ifndef LCD_PROGRESS_ARC_H_\n")
file.write("#define LCD_PROGRESS_ARC_H_\n\n")
file.write("namespace lcd\n{\n\n")
file.write("static const uint8_t NUMBER_OF_ARC_POSITIONS = {0};\n\n".format(NUMBER_OF_POSITIONS))
file.write("static const uint8_t progressArcArray[NUMBER_OF_ARC_POSITIONS][{0}] = {{\n".format((IMAGE_HEIGHT * IMAGE_WIDTH / 8)))

for position in xrange(0, NUMBER_OF_POSITIONS):
    image = Image.new('1', (32 * N_RESIZE, 32 * N_RESIZE), 'white')
    draw = ImageDraw.Draw(image)
    draw.ellipse((0, 0, 32 * N_RESIZE, 32 * N_RESIZE), fill ='black', outline ='black')
    draw.ellipse((5 * N_RESIZE, 5 * N_RESIZE, 27 * N_RESIZE, 27 * N_RESIZE), fill ='white', outline ='white')
    draw.pieslice((0, 0, 32 * N_RESIZE, 32 * N_RESIZE), (-225 + position*270/(NUMBER_OF_POSITIONS-1)), 135, fill ='white', outline ='white')
    image = image.resize((IMAGE_WIDTH,IMAGE_HEIGHT))
    pixelData = image.getdata()
    outputArray = []

    for y in xrange(0, IMAGE_HEIGHT / 8):
        for x in xrange(0, IMAGE_WIDTH):
            currentByte = 0
            for bit in xrange(0, 8):
                currentByte |= (1 if (0 == pixelData[x + (y*8+bit)*IMAGE_WIDTH]) else 0) << bit
            outputArray.append(currentByte)

    #print outputArray

    file.write("        { ")
    byteCount = 0
    for byte in outputArray:
        file.write(("0x%02X" % byte) + ", ")
        byteCount += 1
        if (16 == byteCount):
            byteCount = 0
            file.write("\n        ")
    file.write(" },\n")

file.write("};\n")
file.write("} // namespace\n#endif\n")
file.close()

image.save('test.png')