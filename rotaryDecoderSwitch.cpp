//
//    FILE: rotaryDecoderSwitch.cpp
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.3
//    DATE: 2021-05-17
// PURPOSE: Arduino library for rotary decoder (with switch)
//     URL: https://github.com/RobTillaart/rotaryDecoderSwitch


#include "rotaryDecoderSwitch.h"


/////////////////////////////////////////////////////
//
//  CONSTRUCTORS
//
rotaryDecoderSwitch::rotaryDecoderSwitch(const int8_t address, TwoWire *wire)
{
  _address = address;
  _wire = wire;
}


#if defined (ESP8266) || defined(ESP32)
bool rotaryDecoderSwitch::begin(uint8_t sda, uint8_t scl, uint8_t count)
{
  _count = count;
  if (_count > 2) _count = 2;
  _wire = &Wire;
  _wire->begin(sda, scl);
  if (! isConnected()) return false;
  return true;
}
#endif


bool rotaryDecoderSwitch::begin(uint8_t count)
{
  _count = count;
  if (_count > 2) _count = 2;
  _wire->begin();
  if (! isConnected()) return false;
  return true;
}


bool rotaryDecoderSwitch::isConnected()
{
  _wire->beginTransmission(_address);
  return ( _wire->endTransmission() == 0);
}


void rotaryDecoderSwitch::readInitialState()
{
  uint8_t value = _read8();
  _lastValue = value;
  //  pin 0,1 and 4,5
  for (uint8_t i = 0; i < _count; i++)
  {
    _lastPos[i] = value & 0x03;
    value >>= 4;
  }
}


bool rotaryDecoderSwitch::checkChange()
{
  uint8_t value = _read8();
  return (_lastValue != value);
}


bool rotaryDecoderSwitch::update()
{
  uint8_t value = _read8();
  if (_lastValue == value) 
  {
    return false;
  }
 _lastValue = value;
  for (uint8_t i = 0; i < _count; i++, value >>= 4)
  {
    uint8_t currentpos = (value & 0x03);
    uint8_t change = (_lastPos[i] << 2) | currentpos;
    switch (change)
    {
      case 0b0001:  //  fall through..
      case 0b0111:
      case 0b1110:
      case 0b1000:
        _encoder[i]++;
        break;
      case 0b0010:
      case 0b0100:
      case 0b1101:
      case 0b1011:
        _encoder[i]--;
        break;
    }
    _lastPos[i] = currentpos;
  }
  return true;
}


bool rotaryDecoderSwitch::updateSingle()
{
  uint8_t value = _read8();
  if (_lastValue == value) 
  {
    return false;
  }
 _lastValue = value;

  for (uint8_t i = 0; i < _count; i++, value >>= 4)
  {
    uint8_t currentpos = (value & 0x03);
    uint8_t change = (_lastPos[i] << 2) | currentpos;
    switch (change)
    {
      case 0b0001:  //  fall through..
      case 0b0111:
      case 0b1110:
      case 0b1000:
        _encoder[i] += 1;
        break;
      case 0b0011:
      case 0b0110:
      case 0b1001:
      case 0b1100:
        _encoder[i] += 2;
        break;
      case 0b0010:
      case 0b0100:
      case 0b1101:
      case 0b1011:
        _encoder[i] += 3;
        break;
    }
    _lastPos[i] = currentpos;
  }
  return true;
}


bool rotaryDecoderSwitch::isKeyPressed(uint8_t re)
{
  uint8_t mask = 0x04;
  if (re > 0) mask = 0x40;
  return (_lastValue & mask) == 0;
}


/////////////////////////////////////////////////////
//
//  PRIVATE
//
uint8_t rotaryDecoderSwitch::_read8()
{
  _wire->requestFrom(_address, (uint8_t)1);
  return _wire->read();
}


// -- END OF FILE --

