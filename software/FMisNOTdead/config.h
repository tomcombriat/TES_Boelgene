#ifndef _CONFIG_
#define _CONFIG_




// Config values, do not change
#define PROTO_V1 1
#define PROTO_V2 2
///////////////



#define HARDWARE_VERSION PROTO_V2

#if (HARDWARE_VERSION == PROTO_V1) // big wood one
#define PRESSURE0_MIN 22
#define PRESSURE0_MAX 2380

#define PRESSURE1_MIN 0
#define PRESSURE1_MAX 640

#define PRESSURE2_MIN 0
#define PRESSURE2_MAX 635

#define PRESSURE3_MIN 0
#define PRESSURE3_MAX 750


#elif (HARDWARE_VERSION == PROTO_V2) // small wood one
#define PRESSURE0_MIN 900
#define PRESSURE0_MAX 3060

#define PRESSURE1_MIN 100
#define PRESSURE1_MAX 700

#define PRESSURE2_MIN 50
#define PRESSURE2_MAX 580

#define PRESSURE3_MIN 0
#define PRESSURE3_MAX 600
#endif


#define FM_RATIO_MOD_FREE

#ifdef FM_RATIO_MOD_FREE
//#define LUT_FM_RATIO_FULL // add a LUT to the modRatio which flattens on every full integer values, can also be LUT_FM_RATIO_HALF
#define LUT_FM_RATIO_HALF
#define LUT_FM_RATIO_TAME 1
#endif


#endif