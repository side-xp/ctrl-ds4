/**
 * @file CtrlTemplate.h
 * @brief Main header of the CtrlTemplate library.
 * @todo Describe what the library does.
 */

#pragma once

#include <Arduino.h>

/**
 * @brief Entry point of the library.
 *
 * Call begin() once from setup(), then update() on every iteration of loop().
 *
 * @code
 * CtrlTemplate ctrlTemplate;
 *
 * void setup() {
 *   ctrlTemplate.begin();
 * }
 *
 * void loop() {
 *   ctrlTemplate.update();
 * }
 * @endcode
 *
 * @todo Rename the class and this file to match the library name.
 */
class CtrlTemplate {
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
