# Building instructions and details

This document contains details and building information for Bølgene v1.1.

## General information

This version of Bølgene is a small step from the v1 which was based on a one-sided PCB.
As the previous version, this is based on a Raspberry Pico (RP2040) MCU and a PT8211 DAC. Because of the low number of analog inputs on the RP2040, a companion ADC is also used.

## Audio modes

The PCB supports three audio modes:

 - PWM straight from the RP2040. Resolution is 12bits. All DAC components can then be omitted (PT8211, C9, C10, C11 and C12) and normally C2, C3 and R1.
 - mono output: components for the PWM can be omitted (C7 and C8) and components for the right channel (C10, C12). This outputs a 16bits signal on `out_dac1`.
 - stereo output: components for the PWM can be omitted (C7 and C8). This outputs a 16bits signal on `out_dac1` and `out_dac2`.
 
All outputs do not have volume control as of the PCB. An external log potentiometer can be adjucted.

## Analog inputs

There are a few analog signals that are measured:

### Pressure (touch):

These are measured wih the help of resisor dividers (RV1-4) and are to be connected to force sensitive resistors (range 1.5kg, ref in BOM). The pots are adjusting the sensitivity of the pressure sensors.

## Ribbon

This is to be connected to the central pin of a 10 turns, 10k linear potentiometer. The other two pins should be connected to GNDA and ADC_ref. 

## Synthesis input

There are currently 5 potentiometers to adjust the synthesis parameters. There is a free slot so a sixth could be added.

The test points labelled pot1-5 are to be connected to the central pin (10k linear), and the other two to GNDA and ADC_ref.

## Power

As of now, the system is powered by four AA batteries giving a nominal voltage of 4*1.2 = 4.8V. The only limitation there are the IC voltage tolerances: RP2040 is given fine up to 6V, PT8211 up to 7V.
