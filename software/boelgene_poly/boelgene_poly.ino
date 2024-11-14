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
#include <tables/triangle2048_int8.h>
#include <tables/cos2048_int8.h>
#include <ResonantFilter.h>
#include <FixMathMapper.h>
#include <AudioDelayFeedback.h>

#include <MetaOscil.h>
#include "Oscils.h"

#include <SPI.h>
#include <MCP3XXX.h>


FixMathMapper<UFix<12, 0>, UFix<7, 9>> mapper;

// ADC
MCP3XXX_<10, 8, 300000> adc;


// Pin mapping
const uint8_t pot0 = 3, pot1 = 4, pot2 = 5, pot3 = 6, pot4 = 7;
const uint8_t pitch_pin = 27, octm_pin = 9, octp_pin = 8, refm_pin = 7, refp_pin = 3, led_pin = 6;
const uint8_t pressure_pin1 = 26, pressure_pin2 = 0, pressure_pin3 = 1, pressure_pin4 = 2;

// Octaves plus and minus buttons
Button octm(octm_pin, false);
Button octp(octp_pin, false);

// Global synthesis variables
int octave = 0;
uint16_t volume = 0;
UFix<7, 9> midi_base_note;
UFix<12, 0> pitch_pot_min = 433, pitch_pot_max = 3811;
bool delay_on = false;
uint16_t resonance = 0;  //, cutoff = 0;
uint8_t vol_on_cutoff;


unsigned long last_update_time = 0;



#define N_VOICES 4
#define OSC_NUM_CELLS 2048
struct voice {
  Oscil<OSC_NUM_CELLS, AUDIO_RATE> aSaw;
  Oscil<OSC_NUM_CELLS, AUDIO_RATE> aSub;
  AudioDelayFeedback<2048> aDel;
  SFix<4, 0> transpose;
  uint8_t volume;
  LowPassFilter16 lpf;
  uint16_t cutoff;
};

voice voices[N_VOICES];

void setup() {

  // Voices initialization
  for (int i = 0; i < N_VOICES; i++) {
    voices[i].aSaw.setTable(SAW2048_DATA);
    voices[i].aSub.setTable(COS2048_DATA);
  }
  voices[0].transpose = 0;
  voices[1].transpose = 4;
  voices[2].transpose = 3;
  voices[3].transpose = 7;

  pinMode(pressure_pin1, INPUT);
  pinMode(pitch_pin, INPUT);

  pinMode(refp_pin, INPUT_PULLUP);
  pinMode(led_pin, OUTPUT);

  // Mapper from rotary pot value to midi note (from FixMath)
  mapper.setBounds(pitch_pot_min, pitch_pot_max, 48, 48 + 24);



  startMozzi(MOZZI_CONTROL_RATE);
  pinMode(refm_pin, INPUT_PULLUP);
  pinMode(refp_pin, INPUT_PULLUP);  // otherwise the pin3 is erratic, why? Might be worth an issue.
  pinMode(octm_pin, INPUT_PULLUP);  // otherwise the pin3 is erratic, why? Might be worth an issue.
  pinMode(octp_pin, INPUT_PULLUP);  // otherwise the pin3 is erratic, why? Might be worth an issue.
  pinMode(led_pin,OUTPUT);
  digitalWrite(led_pin, HIGH);
}

void setup1() {
 // Serial.begin(115200);
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
    uint16_t cutoff_read = adc.analogRead(pot0);
    if (cutoff_read < 512)
      for (uint8_t i = 0; i < N_VOICES; i++) voices[i].cutoff = cutoff_read << 6;
    else
      for (uint8_t i = 0; i < N_VOICES; i++) voices[i].cutoff = voices[i].volume * ((cutoff_read - 512) >> 1);


    // Delay Feedback parameter checkng
    int16_t feedback = (int16_t(adc.analogRead(pot2) - 512)) >> 2;
    for (uint8_t i = 0; i < N_VOICES; i++) voices[i].aDel.setFeedbackLevel(feedback);

    uint16_t delay = uint16_t(adc.analogRead(pot3) << 1);
    if (delay < 10) delay_on = false;
    else {
      delay_on = true;
      for (uint8_t i = 0; i < N_VOICES; i++) voices[i].aDel.setDelayTimeCells(uint16_t(delay - 10));
    }

    // Volume checking for additionnal buttons (here because async is not available)
    voices[1].volume = adc.analogRead(pressure_pin2) >> 2;
    voices[2].volume = adc.analogRead(pressure_pin3) >> 2;
    voices[3].volume = adc.analogRead(pressure_pin4) >> 2;
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


  for (uint8_t i = 0; i < N_VOICES; i++) {
    voices[i].aSaw.setFreq(mtof(midi_base_note + voices[i].transpose));
    voices[i].aSub.setFreq(mtof(midi_base_note + voices[i].transpose - SFixAuto<12>()));
  }


  voices[0].volume = mozziAnalogRead<8>(pressure_pin1);

  for (uint8_t i = 0; i < N_VOICES; i++) voices[i].lpf.setCutoffFreqAndResonance(voices[i].cutoff, resonance);
}

AudioOutput updateAudio() {
  int32_t sample = 0;
  for (uint8_t i = 0; i < N_VOICES; i++) {
    int32_t sub_sample = (voices[i].aSaw.next() + voices[i].aSub.next()) >> 1;
    if (delay_on) sub_sample = voices[i].aDel.next(sub_sample);
    sub_sample = voices[i].lpf.next(sub_sample * voices[i].volume);
    sample += sub_sample;
  }
  return MonoOutput::fromNBit(17, sample).clip();
}
