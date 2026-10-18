/**
 * @file UniqueId.h
 * @brief Factory unique ID of the board, and the MAC address derived from it.
 */

#pragma once

#include <stdint.h>

#include "Types.h"

namespace CtrlDS4 {
namespace internal {

/// Size of the largest unique ID among the supported chips (the 128-bit serial number of the SAMD chips).
constexpr uint8_t UNIQUE_ID_MAX_SIZE = 16;

/**
 * @brief Reads the factory unique ID of the board.
 *
 * On SAMD21 and SAMD51 chips, it's the serial number of the chip. On RP2040 chips, it's the ID of
 * the external flash chip, as provided by the Pico SDK: some clone boards have flash chips without
 * a proper ID, in which case it may be blank, or shared with other boards.
 *
 * @param id Buffer receiving the ID.
 * @return Size of the ID in bytes, or 0 if the board has no usable unique ID.
 */
uint8_t readUniqueId(uint8_t (&id)[UNIQUE_ID_MAX_SIZE]);

/**
 * @brief Derives a MAC address from a unique ID.
 *
 * The ID is hashed down to the size of a MAC address, then marked as a locally administered unicast
 * address, so it can't match the address of a real device.
 *
 * @param id Unique ID of the board.
 * @param size Size of the ID in bytes.
 * @return The MAC address, always the same for the same ID.
 */
MacAddress makeMacAddress(const uint8_t *id, uint8_t size);

}  // namespace internal
}  // namespace CtrlDS4
