#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt




def simple_mod(t, f, ratio, modulation):
    return 127.*np.cos(2*np.pi*f*t+ 2*np.pi/65536. * modulation*127*np.cos(2*np.pi*ratio*f*t))

def double_mod(t,f,ratio1, modulation1, ratio2, modulation2):
    mod = simple_mod(t,f*ratio1,ratio2/ratio1, modulation2)
    return 127.*np.cos(2*np.pi*f*t+ 2*np.pi/65536* modulation1 * mod)



def max_alias(fft_freq, fft, ratio):
    return np.amax(fft_freq[np.abs(fft) > np.amax(np.abs(fft))/ratio_to_fund])


sampling = 100e3
length = 1
ratio_to_fund = 10

t = np.arange(0,length,1/sampling)
y = np.cos(2*np.pi*t*1)

plt.plot(t,simple_mod(t,2,1,4096))
#plt.plot(t,simple_mod(t,2,1,2048))
#plt.plot(t,double_mod(t,2,1,4096,0.5,128))
#plt.plot(t,double_mod(t,2,1,4096,1,128))
plt.show()


#fft = np.fft.fft(simple_mod(t,440,2,1))
fft = np.fft.fft(double_mod(t,440,2,1,1,0.1))
fft_freq = np.fft.fftfreq(len(t),1/sampling)

plt.plot(fft_freq, np.abs(fft))
plt.show()

#print("Max aliased generated: ", np.amax(fft_freq[np.abs(fft) > np.amax(np.abs(fft))/ratio_to_fund]))
print("Max alias:", max_alias(fft_freq,fft,10))



mod1 = np.linspace(0,4096)

freq = 220
maxx=[]
maxx2 = []

for i in mod1:
    fft = np.fft.fft(double_mod(t,220,1,i,0.5,32))
    maxx.append(max_alias(fft_freq,fft,10))
    fft = np.fft.fft(double_mod(t,220,2,i,0.5,32))
    maxx2.append(max_alias(fft_freq,fft,10))
plt.plot(mod1, np.array(maxx)/220 )
plt.plot(mod1, np.array(maxx2)/220 )
plt.show()
