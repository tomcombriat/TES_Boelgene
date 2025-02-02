/*
Copyright 2024 Thomas Combriat

This is licensed under the GPL Licence

This is the main code for the poly-Boelgene, a polyphonic version
of a small Ondes Nartenot replica.

It features four pressure buttons instead of one, allowing
different notes to be played simulteanously.
*/

#include "MozziConfigValues.h"  // for named option values
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_I2S_DAC
#define MOZZI_I2S_FORMAT MOZZI_I2S_FORMAT_LSBJ
#define MOZZI_AUDIO_CHANNELS 1
#define MOZZI_CONTROL_RATE 256  // Hz, powers of 2 are most reliable

#include <Button.h>  // from TES_eSax-lib
#include <Mozzi.h>
#include <mozzi_midi.h>
#include <Oscil.h>
#include <tables/saw2048_int8.h>
//#include <tables/triangle2048_int8.h>
#include <tables/cos2048_int8.h>
#include <ResonantFilter.h>
#include <FixMathMapper.h>
//#include <AudioDelayFeedback.h>
#include "BiPot.h"
#include <MetaOscil.h>
#include "Oscils.h"

#include <SPI.h>
#include <MCP3XXX.h>


FixMathMapper<UFix<12, 0>, UFix<7, 9>> mapper;

// ADC
MCP3XXX_<10, 8, 300000> adc;


// Pin mapping
const uint8_t pot0 = 3, pot1 = 4, pot2 = 5, pot3 = 6, pot4 = 7, pot5 = 28;
const uint8_t pitch_pin = 27, octm_pin = 9, octp_pin = 8, refm_pin = 7, refp_pin = 3, led_pin = 6;
const uint8_t pressure_pin1 = 26, pressure_pin2 = 0, pressure_pin3 = 1, pressure_pin4 = 2;

// Octaves plus and minus buttons
Button octm(octm_pin, false);
Button octp(octp_pin, false);

// Global synthesis variables
int octave = 0;
UFix<7, 9> midi_base_note;
UFix<12, 0> pitch_pot_min = 433, pitch_pot_max = 3811;
bool delay_on = false;
uint16_t resonance = 0;  //, cutoff = 0;
UFix<2, 8> mod1Ratio, prevMod1Ratio;
UFix<2, 8> mod2Ratio, prevMod2Ratio;




unsigned long last_update_time = 0;

BiPotMult cutoffPot;
BiPotMult mod1Pot;
BiPotMult mod1RatioPot;
BiPotMult mod2Pot;
BiPotMult mod2RatioPot;


#define N_VOICES 4
#define OSC_NUM_CELLS 2048
struct voice {
  Oscil<OSC_NUM_CELLS, AUDIO_RATE> aCos;
  Oscil<OSC_NUM_CELLS, AUDIO_RATE> aMod1;
  Oscil<OSC_NUM_CELLS, AUDIO_RATE> aMod2;
  Oscil<OSC_NUM_CELLS, AUDIO_RATE> aSub;
  //AudioDelayFeedback<2048> aDel;
  SFix<4, 0> transpose;
  uint8_t volume;
  LowPassFilter16 lpf;
  uint16_t cutoff;
  uint16_t modulation_amount1, modulation_amount2;
};

voice voices[N_VOICES];

void setup() {

  // Voices initialization
  for (int i = 0; i < N_VOICES; i++) {
    //voices[i].aSaw.setTable(SAW2048_DATA);
    voices[i].aCos.setTable(COS2048_DATA);
    voices[i].aMod1.setTable(COS2048_DATA);
    voices[i].aMod2.setTable(COS2048_DATA);
    voices[i].aSub.setTable(COS2048_DATA);
  }
  voices[0].transpose = 0;
  voices[1].transpose = 4;
  voices[2].transpose = 3;
  voices[3].transpose = 7;

  pinMode(pressure_pin1, INPUT);
  pinMode(pitch_pin, INPUT);
  pinMode(pot5, INPUT);

  pinMode(refp_pin, INPUT_PULLUP);
  pinMode(led_pin, OUTPUT);

  // Mapper from rotary pot value to midi note (from FixMath)
  mapper.setBounds(pitch_pot_min, pitch_pot_max, 48, 48 + 24);



  startMozzi(MOZZI_CONTROL_RATE);
  pinMode(refm_pin, INPUT_PULLUP);
  pinMode(refp_pin, INPUT_PULLUP);  // otherwise the pin3 is erratic, why? Might be worth an issue.
  pinMode(octm_pin, INPUT_PULLUP);  // otherwise the pin3 is erratic, why? Might be worth an issue.
  pinMode(octp_pin, INPUT_PULLUP);  // otherwise the pin3 is erratic, why? Might be worth an issue.
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, HIGH);
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
    if (octave > 4) octave = 4;
  }

  // Parameters checking
  if (last_update_time + 2 < millis()) {
    last_update_time = millis();

    // LPF parameters checking
    resonance = adc.analogRead(pot1) << 6;
    cutoffPot.setValue(adc.analogRead(pot0) << 6);
    for (uint8_t i = 0; i < N_VOICES; i++) voices[i].cutoff = cutoffPot.getValue(voices[i].volume << 8);


    //// MODULATION AMOUNTS
    mod1Pot.setValue(adc.analogRead(pot3) << 6);
    for (uint8_t i = 0; i < N_VOICES; i++) voices[i].modulation_amount1 = mod1Pot.getValue(voices[i].volume << 8);

    //mod2Pot.setValue(adc.analogRead(pot5) << 6);
    for (uint8_t i = 0; i < N_VOICES; i++) voices[i].modulation_amount2 = mod2Pot.getValue(voices[i].volume << 8);


    //// MODULATION RATIOS
    mod1RatioPot.setValue(adc.analogRead(pot2) << 6);
    if (mod1RatioPot.getMult()) mod1Ratio = mod1Ratio.fromRaw(mod1RatioPot.getValueRaw() >> 6);
    else {  // constrained ratio
      mod1Ratio = UFix<2, 1>(mod1Ratio.fromRaw(mod1RatioPot.getValueRaw() >> 6));
      if (prevMod1Ratio != mod1Ratio) {
        prevMod1Ratio = mod1Ratio;
        for (uint8_t i = 0; i < N_VOICES; i++) {  // re-align the phase (pointless in free mod)
          voices[i].aCos.setPhaseFractional(0);
          voices[i].aMod1.setPhaseFractional(0);
          voices[i].aMod2.setPhaseFractional(0);
          //voices[i].aMod1.setPhaseFractional(voices[i].aCos.getPhaseFractional());
          //voices[i].aMod2.setPhaseFractional(voices[i].aCos.getPhaseFractional());
        }
      }
    }

    mod2RatioPot.setValue(adc.analogRead(pot4) << 6);
    if (mod2RatioPot.getMult()) mod2Ratio = mod2Ratio.fromRaw(mod2RatioPot.getValueRaw() >> 6);
    else {  // constrained ratio
      mod2Ratio = UFix<2, 1>(mod2Ratio.fromRaw(mod2RatioPot.getValueRaw() >> 6));
      if (prevMod2Ratio != mod2Ratio) {
        prevMod2Ratio = mod2Ratio;
        for (uint8_t i = 0; i < N_VOICES; i++) {  // re-align the phase (pointless in free mod)
          voices[i].aCos.setPhaseFractional(0);
          voices[i].aMod1.setPhaseFractional(0);
          voices[i].aMod2.setPhaseFractional(0);
          //voices[i].aMod2.setPhaseFractional(voices[i].aCos.getPhaseFractional());
          //voices[i].aMod1.setPhaseFractional(voices[i].aCos.getPhaseFractional());
        }
      }
    }
  }

  // Volume checking for additionnal buttons (here because async is not available)
  voices[1].volume = adc.analogRead(pressure_pin2) >> 2;
  voices[2].volume = adc.analogRead(pressure_pin3) >> 2;
  voices[3].volume = adc.analogRead(pressure_pin4) >> 2;

  //Serial.println(mod1Ratio.asFloat());
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
  mod2Pot.setValue(mozziAnalogRead<16>(pot5));

  for (uint8_t i = 0; i < N_VOICES; i++) {
    auto freq = mtof(midi_base_note + voices[i].transpose);
    voices[i].aCos.setFreq(freq);
    voices[i].aSub.setFreq(freq.sR<1>());
    voices[i].aMod1.setFreq(freq * mod1Ratio);
    voices[i].aMod2.setFreq(freq * mod2Ratio);
    //voices[i].aSub.setFreq(mtof(midi_base_note + voices[i].transpose - SFixAuto<12>()));
  }


  voices[0].volume = mozziAnalogRead<8>(pressure_pin1);

  for (uint8_t i = 0; i < N_VOICES; i++) voices[i].lpf.setCutoffFreqAndResonance(voices[i].cutoff, resonance);
}

AudioOutput updateAudio() {
  int32_t sample = 0;
  if (mod1Ratio != UFix<2, 8>(0) && mod2Ratio != UFix<2, 8>(0)) {
    for (uint8_t i = 0; i < N_VOICES; i++) {
      int32_t sub_sample = voices[i].aSub.next() + voices[i].aCos.phMod((voices[i].modulation_amount1 * voices[i].aMod1.phMod((voices[i].modulation_amount2 * voices[i].aMod2.next()) >> 7)) >> 4);
      sub_sample = voices[i].lpf.next(sub_sample * voices[i].volume);
      sample += sub_sample;
    }
  } else if (mod2Ratio == UFix<2, 8>(0)) {
    for (uint8_t i = 0; i < N_VOICES; i++) {
      int32_t sub_sample = voices[i].aSub.next() + voices[i].aCos.phMod((voices[i].modulation_amount1 * voices[i].aMod1.next()) >> 4);
      sub_sample = voices[i].lpf.next(sub_sample * voices[i].volume);
      sample += sub_sample;
    }
  } else {
    for (uint8_t i = 0; i < N_VOICES; i++) {
      int32_t sub_sample = voices[i].aSub.next() + voices[i].aCos.next();
      sub_sample = voices[i].lpf.next(sub_sample * voices[i].volume);
      sample += sub_sample;
    }
  }
  return MonoOutput::fromNBit(17, sample).clip();
}
