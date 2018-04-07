from PIL import Image
import sys
im = Image.open("l1header_2016.png")
pix = im.load()
for r in range(0,40):
	for c in range(0,40):
		sys.stdout.write(str(hex(pix[r,c][0]))+',')
		#if(pix[r,c][0] > 127):
		#	sys.stdout.write(str(hex(255))+',')
		#else:
		#	sys.stdout.write(str(hex(0))+',')