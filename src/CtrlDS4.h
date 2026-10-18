/**
 * @file CtrlDS4.h
 * @brief Main header of the CtrlDS4 library.
 *
 * Emulates a DualShock 4 controller from boards with native USB, such as the RP2040 or the SAMD21.
 */

#pragma once

#include <Arduino.h>

#include "CtrlDS4/Types.h"

/// Everything the CtrlDS4 library declares: the Controller class and the types of its controls.
namespace CtrlDS4 {

/**
 * @brief Emulated DS4 controller.
 *
 * Call begin() once from setup(), then update() on every iteration of loop().
 *
 * @code
 * CtrlDS4::Controller controller;
 *
 * void setup() {
 *   controller.begin();
 * }
 *
 * void loop() {
 *   controller.update();
 * }
 * @endcode
 */
class Controller {
public:
  /* --- Lifecycle --- */

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

}  // namespace CtrlDS4
