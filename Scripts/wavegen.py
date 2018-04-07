import sys
wave_min = 0
wave_max = 65535
wave_step = 256
counter = wave_min
for i in range(0,wave_step):
	sys.stdout.write(str(hex(counter))+',')
	if (i%6 == 5):
		print ' '
	counter += wave_max/wave_step