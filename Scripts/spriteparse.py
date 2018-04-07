from PIL import Image
import sys
import numpy as np

#im = Image.open("fsociety40.jpg")
#pix = im.load()
#for r in range(0,40):
#	for c in range(0,40):
#		if(pix[r,c][0] > 127):
#			sys.stdout.write(str(hex(255))+',')
#		else:
#			sys.stdout.write(str(hex(0))+',')

#im = Image.open("sonic_small_888_trans.png")
#im = Image.open("intro_large.png")
im = Image.open("icon_cadsoft_eagle.png")
pix = im.load()

img_w = 70 #183 #40
img_h = 70 #136 #51 

red = np.uint8(0)
green = np.uint8(0)
blue = np.uint8(0)

compressed_color = np.uint8(0)

color = np.uint16((red << (5 + 6)) | (green << 5) | blue)

color_array = [0,0,0,0,0,0,0,0,0,0,0]
new_color_idx = 0

for r in range(0,img_h):
	for c in range(0,img_w):
		red = np.uint8(pix[c,r][0]) >> 3
		green = np.uint8(pix[c,r][1]) >> 2
		blue = np.uint8(pix[c,r][2]) >> 3
		color = np.uint16((red << (5 + 6)) | (green << 5) | blue)

		
		for cs in range(0,11):
			if(color_array[cs] == color):
				break
			elif(cs == 10):
				color_array[new_color_idx] = color
				if(new_color_idx < 11):
					new_color_idx += 1

		#sys.stdout.write(str(hex(color))+',')
		#print "%d" % pix[r,c][1]

print "Sprite Color Palette:"
for ca in range(0,11):
	print hex(color_array[ca])

for r in range(0,img_h):
	for c in range(0,img_w):
		red = np.uint8(pix[c,r][0]) >> 3
		green = np.uint8(pix[c,r][1]) >> 2
		blue = np.uint8(pix[c,r][2]) >> 3
		color = np.uint16((red << (5 + 6)) | (green << 5) | blue)
		
		sys.stdout.write(str(hex(color_array.index(color)))+',')

# Compressed
'''
for r in range(0,img_h):
	for c in range(0,img_w/2):
		red = np.uint8(pix[c,r][0]) >> 3
		green = np.uint8(pix[c,r][1]) >> 2
		blue = np.uint8(pix[c,r][2]) >> 3
		color = np.uint16((red << (5 + 6)) | (green << 5) | blue)
		color_idx_1 = color_array.index(color)

		red = np.uint8(pix[(2*c)+1,r][0]) >> 3
		green = np.uint8(pix[(2*c)+1,r][1]) >> 2
		blue = np.uint8(pix[(2*c)+1,r][2]) >> 3
		color = np.uint16((red << (5 + 6)) | (green << 5) | blue)
		color_idx_2 = color_array.index(color)

		compressed_color = (color_idx_1 << 4) | color_idx_2
		
		sys.stdout.write(str(hex(compressed_color))+',')
'''
