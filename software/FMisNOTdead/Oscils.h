


// All the tables used for the MetaOscil need to be included

#include <tables/BandLimited_SAW/512/saw_max_138_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 138Hz at a sampling frequency of 16384
#include <tables/BandLimited_SAW/512/saw_max_154_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 154Hz at a sampling frequency of 16384
#include <tables/BandLimited_SAW/512/saw_max_174_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 174Hz at a sampling frequency of 16384
#include <tables/BandLimited_SAW/512/saw_max_210_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 210Hz at a sampling frequency of 16384
#include <tables/BandLimited_SAW/512/saw_max_264_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 264Hz at a sampling frequency of 16384
#include <tables/BandLimited_SAW/512/saw_max_327_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 327Hz at a sampling frequency of 16384
#include <tables/BandLimited_SAW/512/saw_max_431_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 431Hz at a sampling frequency of 16384
#include <tables/BandLimited_SAW/512/saw_max_546_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 546Hz at a sampling frequency of 16384
#include <tables/BandLimited_SAW/512/saw_max_744_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 744Hz at a sampling frequency of 16384
#include <tables/BandLimited_SAW/512/saw_max_910_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 910Hz at a sampling frequency of 16384
#include <tables/BandLimited_SAW/512/saw_max_1170_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 1170Hz at a sampling frequency of 16384
#include <tables/BandLimited_SAW/512/saw_max_1638_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 1638Hz at a sampling frequency of 16384
#include <tables/BandLimited_SAW/512/saw_max_2730_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 2730Hz at a sampling frequency of 16384
#include <tables/BandLimited_SAW/512/saw_max_8192_at_16384_512_int8.h>  // band limited table that guarantee no alias up to a frequency of 8192Hz at a sampling frequency of 16384 (this is basically a sine wave)



Oscil <SAW_MAX_138_AT_16384_512_NUM_CELLS, MOZZI_AUDIO_RATE> aSaw138(SAW_MAX_138_AT_16384_512_DATA);
Oscil <SAW_MAX_154_AT_16384_512_NUM_CELLS, MOZZI_AUDIO_RATE> aSaw154(SAW_MAX_154_AT_16384_512_DATA);
Oscil <SAW_MAX_174_AT_16384_512_NUM_CELLS, MOZZI_AUDIO_RATE> aSaw174(SAW_MAX_174_AT_16384_512_DATA);
Oscil <SAW_MAX_210_AT_16384_512_NUM_CELLS, MOZZI_AUDIO_RATE> aSaw210(SAW_MAX_210_AT_16384_512_DATA);
Oscil <SAW_MAX_264_AT_16384_512_NUM_CELLS, MOZZI_AUDIO_RATE> aSaw264(SAW_MAX_264_AT_16384_512_DATA);
Oscil <SAW_MAX_327_AT_16384_512_NUM_CELLS, MOZZI_AUDIO_RATE> aSaw327(SAW_MAX_327_AT_16384_512_DATA);
Oscil <SAW_MAX_431_AT_16384_512_NUM_CELLS, MOZZI_AUDIO_RATE> aSaw431(SAW_MAX_431_AT_16384_512_DATA);
Oscil <SAW_MAX_546_AT_16384_512_NUM_CELLS, MOZZI_AUDIO_RATE> aSaw546(SAW_MAX_546_AT_16384_512_DATA);
Oscil <SAW_MAX_744_AT_16384_512_NUM_CELLS, MOZZI_AUDIO_RATE> aSaw744(SAW_MAX_744_AT_16384_512_DATA);
Oscil <SAW_MAX_910_AT_16384_512_NUM_CELLS, MOZZI_AUDIO_RATE> aSaw910(SAW_MAX_910_AT_16384_512_DATA);
Oscil <SAW_MAX_1170_AT_16384_512_NUM_CELLS, MOZZI_AUDIO_RATE> aSaw1170(SAW_MAX_1170_AT_16384_512_DATA);
Oscil <SAW_MAX_1638_AT_16384_512_NUM_CELLS, MOZZI_AUDIO_RATE> aSaw1638(SAW_MAX_1638_AT_16384_512_DATA);
Oscil <SAW_MAX_2730_AT_16384_512_NUM_CELLS, MOZZI_AUDIO_RATE> aSaw2730(SAW_MAX_2730_AT_16384_512_DATA);
Oscil <SAW_MAX_8192_AT_16384_512_NUM_CELLS, MOZZI_AUDIO_RATE> aSaw8192(SAW_MAX_8192_AT_16384_512_DATA);