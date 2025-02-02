#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit




def simple_mod(t, f, ratio, modulation):
    return 127.*np.cos(2*np.pi*f*t+ 2*np.pi/65536. * modulation*127*np.cos(2*np.pi*ratio*f*t))

def double_mod(t,f,ratio1, modulation1, ratio2, modulation2):
    mod = simple_mod(t,f*ratio1,ratio2/ratio1, modulation2)
    return 127.*np.cos(2*np.pi*f*t+ 2*np.pi/65536* modulation1 * mod)


def lin_func(x,a):
    return a*x

def lin_intercept_func(x,a,b):
    return a*x+b

def max_alias(fft_freq, fft, ratio):
    return np.amax(fft_freq[np.abs(fft) > np.amax(np.abs(fft))/ratio_to_fund])


sampling = 500e3
length = 1
ratio_to_fund = 10

t = np.arange(0,length,1/sampling)
#y = np.cos(2*np.pi*t*1)

#plt.plot(t,simple_mod(t,2,1,4096))
#plt.plot(t,simple_mod(t,2,1,2048))
#plt.plot(t,double_mod(t,2,1,4096,0.5,128))
#plt.plot(t,double_mod(t,2,1,4096,1,128))
#plt.show()


#fft = np.fft.fft(simple_mod(t,440,2,1))
#fft = np.fft.fft(double_mod(t,440,2,1,1,0.1))
fft_freq = np.fft.fftfreq(len(t),1/sampling)

#plt.plot(fft_freq, np.abs(fft))
#plt.show()

#print("Max aliased generated: ", np.amax(fft_freq[np.abs(fft) > np.amax(np.abs(fft))/ratio_to_fund]))
#print("Max alias:", max_alias(fft_freq,fft,10))

"""
Model = 
f_alias/f0 = A*R1*M1*(1+B*R2*M2)

"""

##### Finding A (putting R2 at 0)

freq = 440 
maxx=[]
maxx2 = []

mod1 = np.linspace(0,4096)
for i in mod1:
    fft = np.fft.fft(double_mod(t,freq,2,i,0,0))
    maxx.append(max_alias(fft_freq,fft,10)/freq)
    fft = np.fft.fft(double_mod(t,freq,1,i,0,0))
    maxx2.append(max_alias(fft_freq,fft,10)/freq)
plt.plot(mod1, np.array(maxx) ) # should be a line of slope A*R1
plt.plot(mod1, np.array(maxx2) )
plt.show()

popt1, pcov1 = curve_fit(lin_func,mod1, maxx)
print("A=",popt1[0] / 2)

popt2, pcov2 = curve_fit(lin_func,mod1, maxx2)
print("A=",popt2[0] / 1)

A = popt2[0]


##### Finding B: f_a/f_0/(A*R1*M1) = 1 + B*R2*M2

mod2 = np.linspace(0, 512)

maxx=[]
maxx2 = []

for i in mod2:
    fft = np.fft.fft(double_mod(t,freq,2,1000,3,i))
    maxx.append(max_alias(fft_freq,fft,10)/freq)
    fft = np.fft.fft(double_mod(t,freq,1,200,0.5,i))
    maxx2.append(max_alias(fft_freq,fft,10)/freq)
plt.plot(mod2, np.array(maxx) ) ## should be a line of slope B*R2
plt.plot(mod2, np.array(maxx2) )
plt.show()

maxx = np.array(maxx)
maxx2 = np.array(maxx2)


popt1, pcov1 = curve_fit(lin_intercept_func,mod2, maxx / (A * 2 * 1000))
print("B=",popt1[0] / 3)
print(popt1[1])

popt1, pcov1 = curve_fit(lin_intercept_func,mod2, maxx2 / (A * 1 * 200))
print("B=",popt1[0] / 0.5)
print(popt1[1])
