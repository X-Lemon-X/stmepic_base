#pragma once

#include "stmepic.hpp"


/**
 * @defgroup hardware Hardware
 * @{
 */

/**
 * @defgroup gpio_hardware_devices GPIO
 * @brief GPIO driver
 * @{
 */


/**
 * @file gpio.hpp
 * @brief Gpio pin definitions for quicker implementation and easy passing to other structures.
 */
namespace stmepic {

/**
 * @struct GpioPin
 * @brief Structure representing the GPIO pin.
 *
 * This structure provides the GPIO pin representation with the port and pin number.
 * and analog value if the pin is analog.
 * This allows to define the pinout configuration in a single file
 * Making code more portable and easier to maintain.
 */

static const uint32_t GPIO_ANALOG_RESOLUTION_8BIT  = 255;
static const uint32_t GPIO_ANALOG_RESOLUTION_10BIT = 1023;
static const uint32_t GPIO_ANALOG_RESOLUTION_12BIT = 4095;
static const uint32_t GPIO_ANALOG_RESOLUTION_14BIT = 16383;
static const uint32_t GPIO_ANALOG_RESOLUTION_16BIT = 65535;
static const uint32_t GPIO_ANALOG_RESOLUTION_24BIT = 


class GpioPinBase {
public:


  /// @brief Write 1 or 0 to the gpio pin
  /// @param value 1 or 0
  void write(uint8_t value);

  /// @brief Read the value of the gpio pin
  /// @return 1 or 0
  uint8_t read();

  /// @brief Toggles the gpio pin from 1->0 or 0->1 respectively
  void toggle();

};

class GpioAnalogBase : public GpioPinBase {
public:

  /// @brief return voltage read on the pin.
  float get_voltage();

  /// @brief Get analog pin value
  uint32_t get_value();

};

} // namespace stmepic
