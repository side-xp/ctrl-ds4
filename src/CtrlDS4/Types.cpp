#include "Types.h"

namespace CtrlDS4 {

// Builds a calibration.
AnalogCalibration::AnalogCalibration(uint16_t minReading, uint16_t maxReading, float deadzone)
  : minReading(minReading), maxReading(maxReading), deadzone(deadzone) {}

// Builds an address from its bytes, in the order they are printed.
MacAddress::MacAddress(uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, uint8_t byte5)
  : bytes{ byte0, byte1, byte2, byte3, byte4, byte5 } {}

// Prints the address as six hexadecimal bytes separated by colons.
size_t MacAddress::printTo(Print &output) const {
  size_t printedCount = 0;
  for (uint8_t i = 0; i < MAC_ADDRESS_SIZE; i++) {
    if (i > 0) {
      printedCount += output.print(':');
    }
    // print() drops the leading zero, so add it to keep two digits per byte.
    if (bytes[i] < 0x10) {
      printedCount += output.print('0');
    }
    printedCount += output.print(bytes[i], HEX);
  }
  return printedCount;
}

}  // namespace CtrlDS4
