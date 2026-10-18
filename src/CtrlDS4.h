/**
 * @file CtrlDS4.h
 * @brief Main header of the CtrlDS4 library.
 *
 * Emulates a DualShock 4 controller from boards with native USB, such as the RP2040 or the SAMD21.
 */

#pragma once

#include <Arduino.h>

/**
 * @brief Entry point of the library.
 *
 * Call begin() once from setup(), then update() on every iteration of loop().
 *
 * @code
 * CtrlDS4 ctrlDS4;
 *
 * void setup() {
 *   ctrlDS4.begin();
 * }
 *
 * void loop() {
 *   ctrlDS4.update();
 * }
 * @endcode
 */
class CtrlDS4 {
public:
  /**
   * @brief Initializes the library.
   *
   * Call once from setup(), before any other method.
   *
   * @return true if initialization succeeded, false otherwise.
   */
  bool begin();

  /**
   * @brief Runs the library logic.
   *
   * Call on every iteration of loop(). Does nothing until begin() has succeeded.
   */
  void update();

  /**
   * @brief Tells whether the library is ready to use.
   *
   * @return true once begin() has completed successfully, false otherwise.
   */
  bool isReady() const;

private:
  /// Set by begin() once initialization has succeeded.
  bool _isReady = false;
};
