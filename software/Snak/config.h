#ifndef _CONFIG_
#define _CONFIG_

#define POLYPHONY 4


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

#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_I2S_DAC
#define MOZZI_I2S_FORMAT MOZZI_I2S_FORMAT_LSBJ

#define REFM_NOTE 48
#define REFP_NOTE REFM_NOTE + 24
#define led_pin 6


#elif (HARDWARE_VERSION == PROTO_V2) // small wood one
#define PRESSURE0_MIN 900
#define PRESSURE0_MAX 3060

#define PRESSURE1_MIN 150
#define PRESSURE1_MAX 700

#define PRESSURE2_MIN 50
#define PRESSURE2_MAX 580

#define PRESSURE3_MIN 0
#define PRESSURE3_MAX 600

#define REFM_NOTE 46
#define REFP_NOTE REFM_NOTE + 19

#define led_pin LED_BUILTIN
#endif


#endif