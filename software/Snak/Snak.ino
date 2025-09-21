/*
Copyright 2024 Thomas Combriat

This is licensed under the GPL Licence

This is the main code for the poly-Boelgene, a polyphonic version
of a small Ondes Nartenot replica.

It features four pressure buttons instead of one, allowing
different notes to be played simulteanously.


TODO: 
 - maybe make BiPot more efficient by changing const values with templates
*/

#include "MozziConfigValues.h"  // for named option values
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_I2S_DAC
#define MOZZI_I2S_FORMAT MOZZI_I2S_FORMAT_LSBJ
#define MOZZI_AUDIO_CHANNELS 1
#define MOZZI_CONTROL_RATE 1024  // Hz, powers of 2 are most reliable

#include <Button.h>  // from TES_eSax-lib
#include <Mozzi.h>
#include <mozzi_midi.h>
#include <Oscil.h>
#include <tables/saw2048_int8.h>
//#include <tables/triangle2048_int8.h>
#include <tables/cos2048_int8.h>
#include <ResonantFilter.h>
#include <FixMathMapper.h>
#include "BiPot.h"
#include <MetaOscil.h>
#include "Oscils.h"
#include <mozzi_rand.h>

#include <SPI.h>
#include <MCP3XXX.h>
#include "config.h"

#if (defined LUT_FM_RATIO_FULL || defined LUT_FM_RATIO_HALF)
#include "modRatioLUT.hpp"
#endif


FixMathMapperFull<UFix<12, 0>, UFix<7, 9>> mapper;
FixMathMapperFull<UFix<12, 0>, UFix<8, 0>, true> pressure0Mapper;
FixMathMapperFull<UFix<10, 0>, UFix<8, 0>, true> pressure1Mapper;
FixMathMapperFull<UFix<10, 0>, UFix<8, 0>, true> pressure2Mapper;
FixMathMapperFull<UFix<10, 0>, UFix<8, 0>, true> pressure3Mapper;

FixMathMapperFull<UFix<12, 0>, UFix<16, 0>, true> bp1Mapper;
FixMathMapperFull<UFix<12, 0>, UFix<16, 0>, true> bp2Mapper;


// ADC
MCP3XXX_<10, 8, 300000> adc;


// Pin mapping
const uint8_t pot0 = 3, pot1 = 4, pot2 = 5, pot3 = 6, pot4 = 7, pot5 = 28;
const uint8_t pitch_pin = 27, octm_pin = 9, octp_pin = 8, refm_pin = 7, refp_pin = 3, led_pin = 6;
const uint8_t pressure_pin0 = 26, pressure_pin1 = 0, pressure_pin2 = 1, pressure_pin3 = 2;

// Octaves plus and minus buttons
Button octm(octm_pin, false);
Button octp(octp_pin, false);

// Global synthesis variables
int octave = 0;
UFix<7, 9> midi_base_note;
UFix<12, 0> pitch_pot_min = 433, pitch_pot_max = 3811;
uint16_t resonance = 65535;  //, cutoff = 0;




unsigned long last_update_time = 0;

/// LED false Osc
uint8_t led_intensity = 0;


// Maybe boost back to 1 once pressure properly calibrated?
BiPotMultBoost cutoffPot(10, 8, 1);
BiPotMultBoost cutoff1Pot(12, 8, 1);
BiPotMultBoost cutoff2Pot(12, 8, 1);


#define N_VOICES 4
#define OSC_NUM_CELLS 512


class voice {
public:  // no offense, most things in public, I know what I am doing
         /* Oscil<OSC_NUM_CELLS, MOZZI_AUDIO_RATE> aCos;
  Oscil<OSC_NUM_CELLS, MOZZI_AUDIO_RATE> aMod1;
  Oscil<OSC_NUM_CELLS, MOZZI_AUDIO_RATE> aMod2;
  Oscil<OSC_NUM_CELLS, MOZZI_AUDIO_RATE> aSub;*/
  SFix<4, 0> transpose;
  MetaOscil<OSC_NUM_CELLS, MOZZI_AUDIO_RATE, 14> aSaw;
  MetaOscil<OSC_NUM_CELLS, MOZZI_AUDIO_RATE, 21> aSq;

  uint8_t volume;
  //LowPassFilter16 lpf;
  ResonantFilter<BANDPASS, uint16_t> bpf1a;
  ResonantFilter<BANDPASS, uint16_t> bpf2a;
  ResonantFilter<BANDPASS, uint16_t> bpf1b;
  ResonantFilter<BANDPASS, uint16_t> bpf2b;

/*
  void setFreq(UFix<16, 16> _freq) {
    aSaw.setFreq(_freq);
  }*/

  void setBaseNote(UFix<7, 9> base_note) {
    freq = mtof(base_note + transpose);
    aSaw.setFreq(freq);
    aSq.setFreq(freq.sR<1>());

  }

  void setCutoff1(uint16_t _cutoff1) {
    cutoff1 = _cutoff1;
    bpf1a.setCutoffFreqAndResonance(cutoff1, resonance);
    bpf1b.setCutoffFreqAndResonance(cutoff1, resonance);
  }

  void setCutoff2(uint16_t _cutoff2) {
    cutoff2 = _cutoff2;
    bpf2a.setCutoffFreqAndResonance(cutoff2, resonance);
    bpf2b.setCutoffFreqAndResonance(cutoff2, resonance);
  }

  void setResonance(uint16_t _resonance) {
    resonance = _resonance;
  }  // to allow potential taming further down the road


  int32_t next() {
    int16_t sample = aSaw.next()+aSq.next();
    int32_t bpfed_sample = volume * (bpf1b.next(bpf1a.next(sample)) + bpf2b.next(bpf2a.next(sample)));
    return bpfed_sample;
  }

private:

  UFix<16, 16> freq;
  uint16_t cutoff1, cutoff2, resonance;
};


voice voices[N_VOICES];

void setup() {

  Serial.begin(115200);
  // Voices initialization

  for (int i = 0; i < POLYPHONY; i++) {
    aSaw154[i].setTable(SAW_MAX_154_AT_16384_512_DATA);
    aSaw182[i].setTable(SAW_MAX_182_AT_16384_512_DATA);
    aSaw221[i].setTable(SAW_MAX_221_AT_16384_512_DATA);
    aSaw282[i].setTable(SAW_MAX_282_AT_16384_512_DATA);
    aSaw356[i].setTable(SAW_MAX_356_AT_16384_512_DATA);
    aSaw431[i].setTable(SAW_MAX_431_AT_16384_512_DATA);
    aSaw546[i].setTable(SAW_MAX_546_AT_16384_512_DATA);
    aSaw630[i].setTable(SAW_MAX_630_AT_16384_512_DATA);
    aSaw744[i].setTable(SAW_MAX_744_AT_16384_512_DATA);
    aSaw910[i].setTable(SAW_MAX_910_AT_16384_512_DATA);
    aSaw1170[i].setTable(SAW_MAX_1170_AT_16384_512_DATA);
    aSaw1638[i].setTable(SAW_MAX_1638_AT_16384_512_DATA);
    aSaw2730[i].setTable(SAW_MAX_2730_AT_16384_512_DATA);
    aSaw8192[i].setTable(SAW_MAX_8192_AT_16384_512_DATA);

    aSq75[i].setTable(SQUARE_MAX_75_AT_16384_512_DATA);
    aSq81[i].setTable(SQUARE_MAX_81_AT_16384_512_DATA);
    aSq88[i].setTable(SQUARE_MAX_88_AT_16384_512_DATA);
    aSq96[i].setTable(SQUARE_MAX_96_AT_16384_512_DATA);
    aSq106[i].setTable(SQUARE_MAX_106_AT_16384_512_DATA);
    aSq118[i].setTable(SQUARE_MAX_118_AT_16384_512_DATA);
    aSq134[i].setTable(SQUARE_MAX_134_AT_16384_512_DATA);
    aSq154[i].setTable(SQUARE_MAX_154_AT_16384_512_DATA);
    aSq182[i].setTable(SQUARE_MAX_182_AT_16384_512_DATA);
    aSq221[i].setTable(SQUARE_MAX_221_AT_16384_512_DATA);
    aSq282[i].setTable(SQUARE_MAX_282_AT_16384_512_DATA);
    aSq356[i].setTable(SQUARE_MAX_356_AT_16384_512_DATA);
    aSq431[i].setTable(SQUARE_MAX_431_AT_16384_512_DATA);
    aSq546[i].setTable(SQUARE_MAX_546_AT_16384_512_DATA);
    aSq630[i].setTable(SQUARE_MAX_630_AT_16384_512_DATA);
    aSq744[i].setTable(SQUARE_MAX_744_AT_16384_512_DATA);
    aSq910[i].setTable(SQUARE_MAX_910_AT_16384_512_DATA);
    aSq1170[i].setTable(SQUARE_MAX_1170_AT_16384_512_DATA);
    aSq1638[i].setTable(SQUARE_MAX_1638_AT_16384_512_DATA);
    aSq2730[i].setTable(SQUARE_MAX_2730_AT_16384_512_DATA);
    aSq8192[i].setTable(SQUARE_MAX_8192_AT_16384_512_DATA);
  }
  for (int i = 0; i < N_VOICES; i++) {
    voices[i].aSaw.setOscils(&aSaw154[i], &aSaw182[i], &aSaw221[i], &aSaw282[i], &aSaw356[i], &aSaw431[i], &aSaw546[i], &aSaw630[i], &aSaw744[i], &aSaw910[i], &aSaw1170[i], &aSaw1638[i], &aSaw2730[i], &aSaw8192[i]);
    voices[i].aSaw.setCutoffFreqs(154 * 2, 182 * 2, 221 * 2, 282 * 2, 356 * 2, 431 * 2, 546 * 2, 630 * 2, 744 * 2, 910 * 2, 1170 * 2, 1638 * 2, 2730 * 2, 8192 * 2);
    voices[i].aSq.setOscils(&aSq75[i], &aSq81[i], &aSq88[i], &aSq96[i], &aSq106[i], &aSq118[i], &aSq134[i], &aSq154[i], &aSq182[i], &aSq221[i],
                            &aSq282[i], &aSq356[i], &aSq431[i], &aSq546[i], &aSq630[i], &aSq744[i], &aSq910[i], &aSq1170[i], &aSq1638[i], &aSq2730[i], &aSq8192[i]);
    voices[i].aSq.setCutoffFreqs(75 * 2, 81 * 2, 88 * 2, 96 * 2, 106 * 2, 118 * 2, 134 * 2, 154 * 2, 182 * 2, 221 * 2, 282 * 2, 356 * 2, 431 * 2,
                                 546 * 2, 630 * 2, 744 * 2, 910 * 2, 1170 * 2, 1638 * 2, 2730 * 2, 8192 * 2);
  }


  voices[0].transpose = 0;
  voices[1].transpose = 4;
  voices[2].transpose = 3;
  voices[3].transpose = 7;




  pinMode(pressure_pin0, INPUT);
  pinMode(pitch_pin, INPUT);
  pinMode(pot5, INPUT);

  pinMode(refp_pin, INPUT_PULLUP);
  pinMode(led_pin, OUTPUT);

  // Mapper from rotary pot value to midi note (from FixMath)
  mapper.setBounds(pitch_pot_min, pitch_pot_max, 48, 48 + 24);

  pressure0Mapper.setBounds(PRESSURE0_MIN, PRESSURE0_MAX, 0, 255);
  pressure1Mapper.setBounds(PRESSURE1_MIN, PRESSURE1_MAX, 0, 255);
  pressure2Mapper.setBounds(PRESSURE2_MIN, PRESSURE2_MAX, 0, 255);
  pressure3Mapper.setBounds(PRESSURE3_MIN, PRESSURE3_MAX, 0, 255);

  bp1Mapper.setBounds(0, 4095, 3136, 9344);
  bp2Mapper.setBounds(0, 4095, 9344, 25600);



  startMozzi(MOZZI_CONTROL_RATE);
  pinMode(refm_pin, INPUT_PULLUP);
  pinMode(refp_pin, INPUT_PULLUP);  // otherwise the pin3 is erratic, why? Might be worth an issue.
  pinMode(octm_pin, INPUT_PULLUP);  // otherwise the pin3 is erratic, why? Might be worth an issue.
  pinMode(octp_pin, INPUT_PULLUP);  // otherwise the pin3 is erratic, why? Might be worth an issue.
  pinMode(led_pin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  analogWriteRange(255);
}

void setup1() {
  //Serial.begin(115200);
  SPI.setRX(16);
  SPI.setTX(19);
  SPI.setSCK(18);
  adc.begin(17);
}

void loop() {
  audioHook();
}

void loop1() {

  // Octave button checking
  octm.update();
  octp.update();

  if (octm.has_been_released()) {
    octave--;
    if (octave < -4) octave = -4;
  }

  if (octp.has_been_released()) {
    octave++;
    if (octave > 3) octave = 3;
  }

  // Parameters checking
  if (last_update_time + 2 < millis()) {
    last_update_time = millis();

    // LPF parameters checking
    resonance = adc.analogRead(pot2) << 6;



    for (uint8_t i = 0; i < N_VOICES; i++) {
      voices[i].setResonance(resonance);
    }
    //cutoff1Pot.setValue(bp1Mapper.map(adc.analogRead(pot0) << 2).asRaw());
    cutoff1Pot.setValue(adc.analogRead(pot0) << 2);
    cutoff2Pot.setValue(adc.analogRead(pot1) << 2);

    // Volume checking for additionnal buttons (here because async is not available)
    voices[1].volume = pressure1Mapper.map(adc.analogRead(pressure_pin1)).asRaw();
    voices[2].volume = pressure2Mapper.map(adc.analogRead(pressure_pin2)).asRaw();
    voices[3].volume = pressure3Mapper.map(adc.analogRead(pressure_pin3)).asRaw();
  }
}


void updateControl() {


  /* Ref min and max */
  if (!digitalRead(refm_pin)) {
    pitch_pot_min = mozziAnalogRead<12>(pitch_pin);
    mapper.setBounds(pitch_pot_min, pitch_pot_max, 48, 48 + 24);
  }
  if (!digitalRead(refp_pin)) {
    pitch_pot_max = mozziAnalogRead<12>(pitch_pin);
    mapper.setBounds(pitch_pot_min, pitch_pot_max, 48, 48 + 24);
  }


  midi_base_note = mapper.map(mozziAnalogRead<12>(pitch_pin));
  midi_base_note = midi_base_note + UFixAuto<12>() * SFix<3, 0>(octave);




  voices[0].volume = pressure0Mapper.map(mozziAnalogRead<12>(pressure_pin0)).asRaw();

  for (uint8_t i = 0; i < N_VOICES; i++) {
    voices[i].setBaseNote(midi_base_note);
    voices[i].setCutoff1(bp1Mapper.map(cutoff1Pot.getValue(voices[i].volume) >> 4).asRaw());
    voices[i].setCutoff2(bp2Mapper.map(cutoff2Pot.getValue(voices[i].volume) >> 4).asRaw());
  }
}

AudioOutput updateAudio() {

  int32_t out = 0;
  for (uint8_t i = 0; i < N_VOICES; i++) {
    out += voices[i].next();
  }


  //int32_t bpfed1 = voices[0].bpf1.next(sample) + voices[0].bpf2.next(sample);

  return MonoOutput::fromNBit(18, out).clip();
}
