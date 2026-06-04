#pragma once

#include "stmepic.hpp"
#include "hardware.hpp"


/**
 * @defgroup hardware Hardware
 * @{
 */

/**
 * @defgroup uart_hardware_devices UART
 * @brief UART driver
 * @{
 */


namespace stmepic {

// struct UartSettings {
//   uint64_t buffer_length = 64; // size of the buffer for the UART RX interface
//   uint64_t queue_size    = 64;
// };


/**
 * @brief Class for controlling the UART interface using  DMA, IT or blocking mode
 * The best mode is DMA and IT since they allows other other tasks to run while the UART is reading or
 * writing all function to read and write data are blocking with timeout
 * .
 */
class UartBase : public HardwareInterface {
public:
  virtual ~UartBase() = default;
  /**
   * @brief Read data from the UART device in blocking mode with other tasks beeing able to freely run in the
   * meantime returns the data read from the device after the read is done
   *
   * @param data the data that will be read from the UART device
   * @param size the size of the data that will be read
   * @param timeout_ms the timeout for the read operation works in all modes.
   * Note its beter to use higher timeout them small one otherwise weird things might happen.
   * @return Result<uint8_t *>
   */
  virtual Status read(uint8_t *data, uint16_t size, uint16_t timeout_ms = 300) = 0;

  /**
   * @brief Write data to the UART device in blocking mode with other tasks beeing able to freely run in the
   *
   * @param data the data that will be written to the UART device
   * @param size the size of the data that will be written
   * @param timeout_ms the timeout for the read operation works in all modes.
   * Note its beter to use higher timeout them small one otherwise weird things might happen.
   * @return Status
   */
  virtual Status write(uint8_t *data, uint16_t size, uint16_t timeout_ms = 100) = 0;
};


} // namespace stmepic