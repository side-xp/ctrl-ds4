#include "UniqueId.h"

#include <string.h>

#if defined(ARDUINO_ARCH_RP2040)
#include <pico/unique_id.h>
#endif

namespace CtrlDS4 {
namespace internal {

namespace {

#if defined(__SAMD51__)
/// Addresses of the four 32-bit words of the serial number (see "Serial Number" in the SAMD5x datasheet).
const uint32_t SERIAL_NUMBER_ADDRESSES[] = { 0x008061FC, 0x00806010, 0x00806014, 0x00806018 };
#elif defined(__SAMD21__)
/// Addresses of the four 32-bit words of the serial number (see "Serial Number" in the SAMD21 datasheet).
const uint32_t SERIAL_NUMBER_ADDRESSES[] = { 0x0080A00C, 0x0080A040, 0x0080A044, 0x0080A048 };
#endif

/// Initial value of the FNV-1a 64-bit hash.
constexpr uint64_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
/// Multiplier of the FNV-1a 64-bit hash.
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

/// Bit of the first byte of a MAC address marking it as locally administered, rather than assigned to a manufacturer.
constexpr uint8_t MAC_LOCALLY_ADMINISTERED = 0x02;
/// Bit of the first byte of a MAC address marking it as multicast, which a device can't use.
constexpr uint8_t MAC_MULTICAST = 0x01;

/**
 * @brief Tells whether an ID holds no information, which is what unprogrammed memory reads as.
 *
 * @param id ID to check.
 * @param size Size of the ID in bytes.
 * @return true if the ID is empty, or all its bits are 0, or all its bits are 1.
 */
bool isBlank(const uint8_t *id, uint8_t size) {
  if (size == 0) {
    return true;
  }
  if (id[0] != 0x00 && id[0] != 0xFF) {
    return false;
  }

  for (uint8_t i = 1; i < size; i++) {
    if (id[i] != id[0]) {
      return false;
    }
  }
  return true;
}

}  // namespace

// Reads the factory unique ID of the board.
uint8_t readUniqueId(uint8_t (&id)[UNIQUE_ID_MAX_SIZE]) {
  uint8_t size = 0;

#if defined(__SAMD21__) || defined(__SAMD51__)
  static_assert(sizeof(SERIAL_NUMBER_ADDRESSES) <= UNIQUE_ID_MAX_SIZE, "The serial number must fit in UNIQUE_ID_MAX_SIZE");
  for (uint32_t address : SERIAL_NUMBER_ADDRESSES) {
    uint32_t word = *reinterpret_cast<const volatile uint32_t *>(address);
    id[size++] = word >> 24;
    id[size++] = word >> 16;
    id[size++] = word >> 8;
    id[size++] = word;
  }
#elif defined(ARDUINO_ARCH_RP2040)
  static_assert(PICO_UNIQUE_BOARD_ID_SIZE_BYTES <= UNIQUE_ID_MAX_SIZE, "The board ID must fit in UNIQUE_ID_MAX_SIZE");
  pico_unique_board_id_t boardId;
  pico_get_unique_board_id(&boardId);
  memcpy(id, boardId.id, PICO_UNIQUE_BOARD_ID_SIZE_BYTES);
  size = PICO_UNIQUE_BOARD_ID_SIZE_BYTES;
#endif

  return isBlank(id, size) ? 0 : size;
}

// Derives a MAC address from a unique ID.
MacAddress makeMacAddress(const uint8_t *id, uint8_t size) {
  uint64_t hash = FNV_OFFSET_BASIS;
  for (uint8_t i = 0; i < size; i++) {
    hash = (hash ^ id[i]) * FNV_PRIME;
  }

  MacAddress macAddress;
  for (uint8_t i = 0; i < MAC_ADDRESS_SIZE; i++) {
    macAddress.bytes[i] = hash >> (i * 8);
  }
  macAddress.bytes[0] = (macAddress.bytes[0] | MAC_LOCALLY_ADMINISTERED) & ~MAC_MULTICAST;
  return macAddress;
}

}  // namespace internal
}  // namespace CtrlDS4
