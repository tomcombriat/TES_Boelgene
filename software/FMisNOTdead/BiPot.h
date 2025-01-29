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

  bool getMult()
  {
    return mult;
  };



private:
  uint16_t value;
  bool mult = false;
};






#endif