#ifndef _PARAMS_
#define _PARAMS_

struct Params {
uint16_t resonance = 65535; 
uint8_t osc_mix=0, sub_level=0;
UFix<2,10> AMfreqRatio=0;
};


#endif