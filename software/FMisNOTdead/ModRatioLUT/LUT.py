#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt



### Flattening LUT curve to make free FM easier to tune on integers values if needed

LUT = np.zeros(256)

for i in range(len(LUT)):
    LUT[i] = np.sin(i*2*np.pi/256)/(2*np.pi/256)

plt.plot(LUT)
plt.show()




#### For full value

x = np.arange(1024)
y1 = np.zeros(1024)
for i in range(len(x)):
    y1[i] = x[i] - LUT[i%256]
plt.plot(x,y1)
plt.show()



### For half value, using the same LUT

y2 = np.zeros(1024)
for i in range(len(x)):
    y2[i] = x[i] - LUT[(2*i)%256]/2
plt.plot(x,y1)
plt.plot(x,y2)
plt.show()



out = open("LUT.hpp","w")
for i in range(len(LUT)):
    if (i%25==0):
        out.write("\n")
    out.write(str(int(np.rint(LUT[i]))))
    out.write(", ")
out.close()

