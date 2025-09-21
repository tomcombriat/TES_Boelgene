#include "config.h"



//////////////////////////////////////
////// SQUARE TABLES

#include <tables/BandLimited_SQUARE/512/square_max_75_at_16384_512_int8.h>
#include <tables/BandLimited_SQUARE/512/square_max_81_at_16384_512_int8.h>
#include <tables/BandLimited_SQUARE/512/square_max_88_at_16384_512_int8.h>
#include <tables/BandLimited_SQUARE/512/square_max_96_at_16384_512_int8.h>
#include <tables/BandLimited_SQUARE/512/square_max_106_at_16384_512_int8.h>
#include <tables/BandLimited_SQUARE/512/square_max_118_at_16384_512_int8.h>
#include <tables/BandLimited_SQUARE/512/square_max_134_at_16384_512_int8.h>
#include <tables/BandLimited_SQUARE/512/square_max_154_at_16384_512_int8.h>
#include <tables/BandLimited_SQUARE/512/square_max_182_at_16384_512_int8.h>
#include <tables/BandLimited_SQUARE/512/square_max_221_at_16384_512_int8.h>
#include <tables/BandLimited_SQUARE/512/square_max_282_at_16384_512_int8.h>
#include <tables/BandLimited_SQUARE/512/square_max_356_at_16384_512_int8.h>
#include <tables/BandLimited_SQUARE/512/square_max_431_at_16384_512_int8.h>
#include <tables/BandLimited_SQUARE/512/square_max_546_at_16384_512_int8.h>
#include <tables/BandLimited_SQUARE/512/square_max_630_at_16384_512_int8.h>
#include <tables/BandLimited_SQUARE/512/square_max_744_at_16384_512_int8.h>
#include <tables/BandLimited_SQUARE/512/square_max_910_at_16384_512_int8.h>
#include <tables/BandLimited_SQUARE/512/square_max_1170_at_16384_512_int8.h>
#include <tables/BandLimited_SQUARE/512/square_max_1638_at_16384_512_int8.h>
#include <tables/BandLimited_SQUARE/512/square_max_2730_at_16384_512_int8.h>
#include <tables/BandLimited_SQUARE/512/square_max_8192_at_16384_512_int8.h>  //21

Oscil<SQUARE_MAX_75_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq75[POLYPHONY];
Oscil<SQUARE_MAX_75_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq81[POLYPHONY];
Oscil<SQUARE_MAX_75_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq88[POLYPHONY];
Oscil<SQUARE_MAX_75_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq96[POLYPHONY];
Oscil<SQUARE_MAX_75_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq106[POLYPHONY];
Oscil<SQUARE_MAX_75_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq118[POLYPHONY];
Oscil<SQUARE_MAX_75_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq134[POLYPHONY];
Oscil<SQUARE_MAX_75_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq154[POLYPHONY];
Oscil<SQUARE_MAX_75_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq182[POLYPHONY];
Oscil<SQUARE_MAX_75_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq221[POLYPHONY];
Oscil<SQUARE_MAX_75_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq282[POLYPHONY];
Oscil<SQUARE_MAX_75_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq356[POLYPHONY];
Oscil<SQUARE_MAX_75_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq431[POLYPHONY];
Oscil<SQUARE_MAX_75_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq546[POLYPHONY];
Oscil<SQUARE_MAX_75_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq630[POLYPHONY];
Oscil<SQUARE_MAX_75_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq744[POLYPHONY];
Oscil<SQUARE_MAX_75_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq910[POLYPHONY];
Oscil<SQUARE_MAX_75_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq1170[POLYPHONY];
Oscil<SQUARE_MAX_75_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq1638[POLYPHONY];
Oscil<SQUARE_MAX_75_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq2730[POLYPHONY];
Oscil<SQUARE_MAX_75_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSq8192[POLYPHONY];



//////////////////////////////////////
//////////// SAW TABLES

#include <tables/BandLimited_SAW/512/saw_max_154_at_16384_512_int8.h>
#include <tables/BandLimited_SAW/512/saw_max_182_at_16384_512_int8.h>
#include <tables/BandLimited_SAW/512/saw_max_221_at_16384_512_int8.h>
#include <tables/BandLimited_SAW/512/saw_max_282_at_16384_512_int8.h>
#include <tables/BandLimited_SAW/512/saw_max_356_at_16384_512_int8.h>
#include <tables/BandLimited_SAW/512/saw_max_431_at_16384_512_int8.h>
#include <tables/BandLimited_SAW/512/saw_max_546_at_16384_512_int8.h>
#include <tables/BandLimited_SAW/512/saw_max_630_at_16384_512_int8.h>
#include <tables/BandLimited_SAW/512/saw_max_744_at_16384_512_int8.h>
#include <tables/BandLimited_SAW/512/saw_max_910_at_16384_512_int8.h>
#include <tables/BandLimited_SAW/512/saw_max_1170_at_16384_512_int8.h>
#include <tables/BandLimited_SAW/512/saw_max_1638_at_16384_512_int8.h>
#include <tables/BandLimited_SAW/512/saw_max_2730_at_16384_512_int8.h>
#include <tables/BandLimited_SAW/512/saw_max_8192_at_16384_512_int8.h>  //14


Oscil<SAW_MAX_154_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSaw154[POLYPHONY];
Oscil<SAW_MAX_154_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSaw182[POLYPHONY];
Oscil<SAW_MAX_154_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSaw221[POLYPHONY];
Oscil<SAW_MAX_154_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSaw282[POLYPHONY];
Oscil<SAW_MAX_154_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSaw356[POLYPHONY];
Oscil<SAW_MAX_154_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSaw431[POLYPHONY];
Oscil<SAW_MAX_154_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSaw546[POLYPHONY];
Oscil<SAW_MAX_154_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSaw630[POLYPHONY];
Oscil<SAW_MAX_154_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSaw744[POLYPHONY];
Oscil<SAW_MAX_154_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSaw910[POLYPHONY];
Oscil<SAW_MAX_154_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSaw1170[POLYPHONY];
Oscil<SAW_MAX_154_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSaw1638[POLYPHONY];
Oscil<SAW_MAX_154_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSaw2730[POLYPHONY];
Oscil<SAW_MAX_154_AT_16384_512_NUM_CELLS, AUDIO_RATE> aSaw8192[POLYPHONY];
