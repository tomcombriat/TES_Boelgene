#include <sys/_stdint.h>
#ifndef __BIPOT__
#define __BIPOT__

/* This defines a simple pot with two zones: first half just returns the value, second returns the value multiplied by n input value.
*/
class BiPotMult {
public:
  BiPotMult(){};

  void setValue(uint16_t val) {
    if (val < 32768) {
      value = val << 1;
      mult = false;
    } else {
      {
        value = (val - 32768) << 1;
        mult = true;
      }
    }
  };

  uint16_t getValue(uint16_t input) {
    if (!mult) return value;
    else return uint16_t((uint32_t(input) * value) >> 16);
  };

  uint16_t getValueRaw() {
    return value;
  };

  bool getMult() {
    return mult;
  };

private:
  uint16_t value;
  bool mult = false;
};


class BiPotMultBoost {
public:
  BiPotMultBoost(uint8_t _nBitsIn = 16, uint8_t _nBitsMult = 16, uint8_t multiplier = 1)
    : nBitsIn(_nBitsIn), nBitsMult(_nBitsMult), multiplier(multiplier){};

  void setValue(uint16_t val) {
    if (val < (1 << (nBitsIn - 1))) {
      value = val << 1;
      mult = false;
    } else {
      value = (val - (1 << (nBitsIn - 1))) << 1;
      mult = true;
    }
  };


  uint16_t getValue(uint16_t input) {
    if (!mult) return value << (16 - nBitsIn);
    // else return uint16_t((uint32_t(input) * value) >> 16);
    else {
      uint32_t ret = (uint32_t(input) * value * multiplier) >> (nBitsIn + nBitsMult - 16);
      if (ret > 65535) return 65535;
      else return uint16_t(ret);
      //return uint16_t((uint32_t(input) * value) >> (nBitsIn + nBitsMult - 16));
    }
  };

  uint16_t getValueRaw() {
    return value;
  };

  bool getMult() {
    return mult;
  };

private:
  uint16_t value;
  bool mult = false;
  const uint8_t nBitsIn, nBitsMult, multiplier;
};




#endif