
[![Arduino CI](https://github.com/RobTillaart/rotaryDecoderSwitch/workflows/Arduino%20CI/badge.svg)](https://github.com/marketplace/actions/arduino_ci)
[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/RobTillaart/rotaryDecoderSwitch/blob/master/LICENSE)
[![GitHub release](https://img.shields.io/github/release/RobTillaart/rotaryDecoderSwitch.svg?maxAge=3600)](https://github.com/RobTillaart/rotaryDecoderSwitch/releases)


# rotaryDecoderSwitch

Arduino library for a PCF8574 based rotary decoder - supports 2 rotary encoders with a switch


## Description

Experimental - not tested extensively

related to https://github.com/RobTillaart/rotaryDecoder
interface is kept same as much as possible.

```cpp
// connect up to 2 rotary encoders with a switch to 1 PCF8574.
//
//  RotaryEncoder    PCF8574      UNO         REMARKS
//  -----------------------------------------------------------
//   1 pin A          pin 0
//   1 pin B          pin 1
//   1 switch         pin 2                   (switch to gnd)
//                    pin 3                   (reserved)
//   2 pin A          pin 4
//   2 pin B          pin 5
//   2 switch         pin 6                   (switch to gnd)
//                    pin 7                   (reserved)
//
//                    SDA         A4
//                    SCL         A5
//
```

Note the above mapping is sort of compatible to using the rotaryDecoder class and using device 0 and 2 only,



## Interface

- **rotaryDecoderSwitch(const int8_t address, TwoWire \*wire = Wire);** constructor to set the address and optional the Wire bus.
- **bool begin(uint8_t sda, uint8_t scl, uint8_t cnt = 2)** ESP32 ea initializes the class.
sets I2C pins.
cnt is the number of rotary encoders connected.
returns true if the PCF8574 is on the I2C bus.
- **bool begin(uint8_t cnt = 2)** UNO ea. initializes the class. 
cnt is the number of rotary encoders connected.
returns true if the PCF8574 is on the I2C bus.
- **bool isConnected()** returns true if the PCF8574 is on the I2C bus.


## Core functions

- **void readInitialState()** read the inital state of the 2 rotary encoders. typically called in setup only, or after a sleep e.g. in combination with **setValue()**
- **bool checkChange()** polling to see if one or more RE have changed, without updating the counters.
- **void update()** update the internal counters of the RE, and the flags if a key is pressed. The counters will add +1 or -1 depending on direction. Need to be called before **getValue()** or before **getKeyPressed()**.
- **void updateSingle()** update the internal counters of the RE. This will add +1 +2 or +3 as it assumes that the rotary encoder only goes into a single direction. 


## Counters & keypresses

- **uint32_t getValue(uint8_r re)** returns the RE counter. (re = 0 or 1).
- **void setValue(uint8_r re, uint32_t val = 0)** (re)set the internal counter to val, default 0
- **bool isKeyPressed(uint8_t re)** returns true is the switch is pressed of the RE (re = 0 or 1). Note one needs to call **update()** first!


## Debugging

- **int8_t getLastPosition(uint8_r re)** (re = 0 or 1).
- **int8_t getRaw()**


## Performance

As the decoder is based upon a PCF8574, a I2C device, the performance is affected by the 
clockspeed of the I2C bus. All four core functions have one call to **\_read()** which is the most expensive part.

Early tests gave the following indicative times (Arduino UNO) for the **update()** 
function. Note that above 500KHz the gain becomes less
while reliability of signal decreases. (500KHz is ~3x faster than 100 KHz)

| I2C speed | time (us) | delta |  %%  |
|:---------:|:---------:|:-----:|:-----:|
| 100 KHz   |    234    |       |       |
| 200 KHz   |    136    |  98   | 42%   |
| 300 KHz   |    100    |  36   | 26%   |
| 400 KHz   |     85    |  15   | 15%   | preferred max
| 500 KHz   |     78    |   7   |  8%   |
| 600 KHz   |     67    |  11   | 14%   | (strange outlier)
| 700 KHz   |     63    |   4   |  6%   |

At @400KHz it can update 2 rotary encoders in less than 90 us. 
At a 50% update percentage this implies a max of about 
5000 **update()** calls per second in theory 
**to be tested in practice**

Note that a high speed drill goes up to 30000 RPM = 500 RPS = 2000 interrupts per second, 
assuming 4 pulses == 360�. (not tested)


## Operational

See examples..

## TODO

- think of what to do with the two "idle lines"
