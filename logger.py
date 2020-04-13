import matplotlib.pyplot as plt

import numpy as np

import serial

import time



Fs = 100.0;  # sampling rate

Ts = 10.0/Fs; # sampling interval

t = np.arange(0,10,Ts) # time vector; create Fs samples between 0 and 1.0 sec.

X = np.arange(0,10,Ts) # signal vector; create Fs samples

Y = np.arange(0,10,Ts) 

Z = np.arange(0,10,Ts)

tilt = np.arange(0,10,Ts)


#n = len(y) # length of the signal

#k = np.arange(n)

#T = n/Fs

#frq = k/T # a vector of frequencies; two sides frequency range

#frq = frq[range(int(n/2))] # one side frequency range



serdev = '/dev/ttyACM0'

s = serial.Serial(serdev,115200)

for a in range(0, int(Fs)):

    line=s.readline() # Read an echo string from K66F terminated with '\n'
    
    linex = line[0:9]
    X[a] = float(linex)

    liney = line[9:18]
    Y[a] = float(liney)

    linez = line[18:27]
    Z[a] = float(linez)

for i in range(100):
    if(abs(X[i])>0.5 or abs(Y[i])>0.5):
        tilt[i] = 1
    else:
        tilt[i] = 0



#Y = np.fft.fft(y)/n*2 # fft computing and normalization

#Y = Y[range(int(n/2))] # remove the conjugate frequency parts



fig, ax = plt.subplots(2, 1)

ax[0].plot(t,X,t,Y,t,Z)

ax[0].legend(('x','y','z'))

ax[0].set_xlabel('Time')

ax[0].set_ylabel('Acc Vector')

ax[1].stem(t,tilt,'b') # plotting the spectrum

ax[1].set_xlabel('Time')

ax[1].set_ylabel('Tilt')

plt.show()

s.close()