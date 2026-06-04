#pragma once

#include "stmepic.hpp"
#include "hardware.hpp"
#include "multiplexer.hpp"
#include <optional>
#include <vector>

/**
 * @defgroup hardware Hardware
 * @{
 */

/**
 * @defgroup i2c_hardware_devices I2C
 * @brief I2C driver
 * @{
 */

namespace stmepic {


class I2cBase : public HardwareInterface {
public:
  /**
   * @brief Read data from the I2C device from memory in blocking mode with other tasks beeing able to freely
   * run in the meantime returns the data read from the device after the read is done
   *
   * @param address the address of the I2C device
   * @param mem_address the memory address from which the data will be read
   * @param data the data that will be read from the I2C device
   * @param size the size of the data that will be read
   * @return Result<uint8_t *>
   */
  virtual Status
  read(uint16_t address, uint16_t mem_address, uint8_t *data, uint16_t size, uint16_t mem_size = 1, uint16_t timeout_ms = 300) = 0;


  /**
   * @brief Write data to the I2C device in blocking mode with other tasks beeing able to freelu run in the
   *
   * @param address the address of the I2C device in 7 bir format it will be automatically by 1
   * @param mem_address the memory address to which the data will be written
   * @param mem_size the size of the memory address
   * @param data the data that will be written to the I2C device
   * @param size the size of the data that will be written
   * @param mem_size the size of the memory address
   * @param timeout_ms the timeout for the write operation
   * @return Status
   */
  virtual Status
  write(uint16_t address, uint16_t mem_address, uint8_t *data, uint16_t size, uint16_t mem_size = 1, uint16_t timeout_ms = 300) = 0;

  /**
   * @brief Check if the device is ready to communicate with specified address
   * @param address the address of the I2C device
   * @param trials the number of trials to check if the device is ready
   * @param timeout the timeout for each trial
   * @return Status
   */
  virtual Status is_device_ready(uint16_t address, uint32_t trials, uint32_t timeout) = 0;

  /**
   * @brief Scan for all I2C devices on the bus
   * @return  the addresses of the devices found on the bus
   */
  [[nodiscard]] virtual Result<std::vector<uint16_t>> scan_for_devices() = 0;
};


class I2cMultiplexerChannel;

class I2cMultiplexerChannel : public I2cBase {
public:
  I2cMultiplexerChannel(std::shared_ptr<I2cBase> i2c, uint8_t channel, MultiplexerBase &multiplexer)
  : _channel(channel), _i2c(i2c), _multiplexer(multiplexer){};
  Status hardware_start() override;
  Status hardware_stop() override;
  Status hardware_reset() override;
  Status read(uint16_t address, uint16_t mem_address, uint8_t *data, uint16_t size, uint16_t mem_size = 1, uint16_t timeout_ms = 300) override;
  Status write(uint16_t address, uint16_t mem_address, uint8_t *data, uint16_t size, uint16_t mem_size = 1, uint16_t timeout_ms = 300) override;
  Status is_device_ready(uint16_t address, uint32_t trials, uint32_t timeout) override;
  Result<std::vector<uint16_t>> scan_for_devices() override;

private:
  uint8_t _channel;
  std::shared_ptr<I2cBase> _i2c;
  MultiplexerBase &_multiplexer;
};

/// @brief Class for using an I2C with a multiplexer with selectable address pins
/// with auto handling of the channel switching depending on the requested channel by the driver using the I2C interface.
class I2cMultiplexerGpioID : public MultiplexerBase {
public:
  /// @brief Make new I2C multiplexer interface with selectable address pins
  ///
  /// @param i2c the I2C interface that will be used to communicate with the multiplexer
  /// @param channels the number of channels of the multiplexer
  /// @param address_pin_1 the first address pin of the multiplexer address pin 1 is mandatory.
  /// @param address_pin_2 the second address pin of the multiplexer
  /// @param address_pin_3 the third address pin of the multiplexer
  /// @param address_pin_4 the fourth address pin of the multiplexer
  /// @param pin_reset optional reset pin for the multiplexer
  static Result<std::shared_ptr<I2cMultiplexerGpioID>> Make(std::shared_ptr<I2cBase> i2c,
                                                            uint8_t channels,
                                                            GpioPinBase* address_pin_1,
                                                            std::optional<GpioPinBase*> address_pin_2 = std::nullopt,
                                                            std::optional<GpioPinBase*> address_pin_3 = std::nullopt,
                                                            std::optional<GpioPinBase*> address_pin_4 = std::nullopt,
                                                            uint8_t switch_delay_us              = 1);

  /// @brief Get the I2C interface for the specific channel,
  /// Which should be passed to the device driver which is connected to the multiplexer on this specific channel.
  /// @param channel the channel number to which the device is connected
  /// @return the I2C interface for the specific channel this should be passed to the device driver which is connected to the multiplexer on the specific channel.
  Result<std::shared_ptr<I2cBase>> get_i2c_interface_for_channel(uint8_t channel);

private:
  I2cMultiplexerGpioID(std::shared_ptr<I2cBase> i2c,
                       GpioPin address_pin_1,
                       std::optional<GpioPinBase*> address_pin_2 = std::nullopt,
                       std::optional<GpioPinBase*> address_pin_3 = std::nullopt,
                       std::optional<GpioPinBase*> address_pin_4 = std::nullopt,
                       uint8_t switch_delay_us              = 10);


  virtual Status select_channel(uint8_t channel) override;
  virtual uint8_t get_selected_channel() const override;
  virtual uint8_t get_total_channels() const override;

  std::shared_ptr<I2cBase> _i2c;
  std::vector<std::shared_ptr<I2cBase>> _i2c_channels;
  uint8_t _channels;
  uint8_t _selected_channel;
  GpioPinBase _address_pin_1;
  std::optional<GpioPinBase*> _address_pin_2;
  std::optional<GpioPinBase*> _address_pin_3;
  std::optional<GpioPinBase*> _address_pin_4;
  uint8_t _switch_delay_us;
};

} // namespace stmepic