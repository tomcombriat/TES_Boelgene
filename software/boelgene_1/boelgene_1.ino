/*
Copyright 2024 Thomas Combriat

This is licensed under the GPL Licence

This is the main code of the first version of Boelgene, a replica of Ondes Martenot
created for Oslo Skaper Festival, 2024.
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

MCP3XXX_<10, 8, 300000> adc;



const uint8_t pressure_pin1 = 26, pot0 = 3, pot1 = 4, pot2 = 5, pot3 = 6, pot4 = 7;
const uint8_t pitch_pin = 27, octm_pin = 9, octp_pin = 8, refm_pin = 7, refp_pin = 3, led_pin = 6;

Button octm(octm_pin, false);
Button octp(octp_pin, false);

int octave = 0;
uint16_t volume = 0;
UFix<7, 9> midi_base_note;
UFix<12, 0> pitch_pot_min = 433, pitch_pot_max = 3811;
bool delay_on = false;

unsigned long last_update_time = 0;

Oscil<SAW2048_NUM_CELLS, AUDIO_RATE> aSaw(SAW2048_DATA);
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aCosSub(COS2048_DATA);

//MetaOscil<SAW_MAX_138_AT_16384_512_NUM_CELLS, MOZZI_AUDIO_RATE,14> aSaw(&aSaw138,&aSaw154,&aSaw174,&aSaw210,&aSaw264,&aSaw327,&aSaw431,&aSaw546,&aSaw744,&aSaw910,&aSaw1170,&aSaw1638,&aSaw2730,&aSaw8192);
LowPassFilter16 lpf;
uint16_t resonance = 0, cutoff = 0;
uint8_t vol_on_cutoff;

AudioDelayFeedback<2048> aDel;

void setup() {


  pinMode(pressure_pin1, INPUT);
  pinMode(pitch_pin, INPUT);

  pinMode(refp_pin, INPUT_PULLUP);
  pinMode(led_pin, OUTPUT);

  lpf.setResonance(resonance);

  mapper.setBounds(pitch_pot_min, pitch_pot_max, 48, 48 + 24);



  startMozzi(MOZZI_CONTROL_RATE);
  pinMode(refm_pin, INPUT_PULLUP);
  pinMode(refp_pin, INPUT_PULLUP);  // otherwise the pin3 is erratic, why? Might be worth an issue.
  pinMode(octm_pin, INPUT_PULLUP);  // otherwise the pin3 is erratic, why? Might be worth an issue.
  pinMode(octp_pin, INPUT_PULLUP);  // otherwise the pin3 is erratic, why? Might be worth an issue.
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

  if (last_update_time + 2 < millis()) {
    last_update_time = millis();
    resonance = adc.analogRead(4) << 6;

    uint16_t cutoff_read = adc.analogRead(3);
    if (cutoff_read < 512) cutoff = cutoff_read << 6;
    else /*vol_on_cutoff = (cutoff_read-1024)<<2;*/
    {
      cutoff = volume * ((cutoff_read - 512) >> 1);
    }
    aDel.setFeedbackLevel((int16_t(adc.analogRead(pot2) - 512)) >> 2);

    uint16_t delay = uint16_t(adc.analogRead(pot3) << 1);


    if (delay < 10) delay_on = false;
    else {
      delay_on = true;
      aDel.setDelayTimeCells(uint16_t(delay - 10));
    }
 /*   Serial.print(pitch_pot_min.asInt());
    Serial.print(" ");
    Serial.println(pitch_pot_max.asInt());*/
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

  aSaw.setFreq(mtof(midi_base_note));
  aCosSub.setFreq(mtof(midi_base_note - SFixAuto<12>()));


  volume = mozziAnalogRead<8>(pressure_pin1);
  //cutoff = volume<<7; //8
  lpf.setCutoffFreqAndResonance(cutoff, resonance);
}

AudioOutput updateAudio() {
  int16_t sample = (aSaw.next() + aCosSub.next()) >> 1;
  if (delay_on) sample = aDel.next(sample);
  return MonoOutput::fromNBit(16, lpf.next(volume * sample)).clip();
}
