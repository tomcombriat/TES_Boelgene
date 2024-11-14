# TES - Bølgene

Bølgene is a digital instrument inspired by the [Ondes Martenot](https://en.wikipedia.org/wiki/Ondes_Martenot), a contemporary instrument to the Theremin.
The original instrument features a force sensitive button for the expression/volume and continuous pitch controlled by the position of a ring.

Bølgene is a small, table-top, digital, instrument having the base features. The main differences is that it features polyphony, with several pressure sensitive buttons.

Bølgene (first version) was created in a few weeks specially for [Oslo Skaper Festival 2024](https://norwaymakers.org/osf24). 

## v1

### Hardware

Nothing special here except a well sized plank, found in the street. Apart from that…
 - CPU: RP2040
 - additionnal ADC: MCP3008
 - DAC: PT8211
 - expressions buttons: 4
 - synthesis potentiometers:

Made on single sided PCB.

### Software
 - sound system: [Mozzi](https://sensorium.github.io/Mozzi/)

#### boelgene_1

The version presented at Oslo Skaper Festival, monophonic with Delay Feedback creating weird sounds, especially in the bass register.

#### boelgene_poly 

Version supporting 4 expression buttons, currently tuned to the fifth and the two thirds. Uses the same synthesis algorithm than boelgene_v1
