#include "i2c.hpp"
#include "stmepic.hpp"

using namespace stmepic;

Status
I2cMultiplexerChannel::hardware_start()
{
  _multiplexer.lock();
  auto ret = _i2c->hardware_start();
  _multiplexer.unlock();
  return ret;
}

Status
I2cMultiplexerChannel::hardware_stop()
{
  _multiplexer.lock();
  auto ret = _i2c->hardware_stop();
  _multiplexer.unlock();
  return ret;
}

Status
I2cMultiplexerChannel::hardware_reset()
{
  _multiplexer.lock();
  auto ret = _i2c->hardware_reset();
  _multiplexer.unlock();
  return ret;
}

Status
I2cMultiplexerChannel::read(uint16_t address,
                            uint16_t mem_address,
                            uint8_t* data,
                            uint16_t size,
                            uint16_t mem_size,
                            uint16_t timeout_ms)
{
  _multiplexer.lock();
  (void)_multiplexer.select_channel(_channel);
  auto ret = _i2c->read(address, mem_address, data, size, mem_size, timeout_ms);
  _multiplexer.unlock();
  return ret;
}

Status
I2cMultiplexerChannel::write(uint16_t address,
                             uint16_t mem_address,
                             uint8_t* data,
                             uint16_t size,
                             uint16_t mem_size,
                             uint16_t timeout_ms)
{
  _multiplexer.lock();
  (void)_multiplexer.select_channel(_channel);
  auto ret = _i2c->write(address, mem_address, data, size, mem_size, timeout_ms);
  _multiplexer.unlock();
  return ret;
}

Status
I2cMultiplexerChannel::is_device_ready(uint16_t address,
                                       uint32_t trials,
                                       uint32_t timeout)
{
  _multiplexer.lock();
  (void)_multiplexer.select_channel(_channel);
  auto ret = _i2c->is_device_ready(address, trials, timeout);
  _multiplexer.unlock();
  return ret;
}

Result<std::vector<uint16_t>>
I2cMultiplexerChannel::scan_for_devices()
{
  _multiplexer.lock();
  (void)_multiplexer.select_channel(_channel);
  auto ret = _i2c->scan_for_devices();
  _multiplexer.unlock();
  return ret;
}

Result<std::shared_ptr<I2cMultiplexerGpioID>>
I2cMultiplexerGpioID::Make(std::shared_ptr<I2cBase> i2c,
                           uint8_t channels,
                           GpioPinBase* address_pin_1,
                           std::optional<GpioPinBase*> address_pin_2,
                           std::optional<GpioPinBase*> address_pin_3,
                           std::optional<GpioPinBase*> address_pin_4,
                           uint8_t switch_delay_us)
{
  if (i2c == nullptr)
    return Status::Invalid("I2C interface is null");
  if (channels < 1 || channels > 16)
    return Status::Invalid("Channels must be between 1 and 16");
  if (channels > 1 && !address_pin_2.has_value())
    return Status::Invalid("Address pin 2 must be provided for more than 1 channel");
  if (channels > 3 && !address_pin_3.has_value())
    return Status::Invalid("Address pin 3 must be provided for more than 3 channels");
  if (channels > 7 && !address_pin_4.has_value())
    return Status::Invalid("Address pin 4 must be provided for more than 7 channels");

  return Result<std::shared_ptr<I2cMultiplexerGpioID>>::OK(
    std::shared_ptr<I2cMultiplexerGpioID>(new I2cMultiplexerGpioID(
      i2c, address_pin_1, address_pin_2, address_pin_3, address_pin_4, switch_delay_us)));
}

I2cMultiplexerGpioID::I2cMultiplexerGpioID(std::shared_ptr<I2cBase> i2c,
                                           GpioPinBase* address_pin_1,
                                           std::optional<GpioPinBase*> address_pin_2,
                                           std::optional<GpioPinBase*> address_pin_3,
                                           std::optional<GpioPinBase*> address_pin_4,
                                           uint8_t switch_delay_us)
  : _i2c(i2c)
  , _address_pin_1(address_pin_1)
  , _address_pin_2(address_pin_2)
  , _address_pin_3(address_pin_3)
  , _address_pin_4(address_pin_4)
  , _channels(0)
  , _selected_channel(1)
  , _switch_delay_us(switch_delay_us)
{
  _channels = 0;
  if (address_pin_2.has_value())
    _channels += 2;
  if (address_pin_3.has_value())
    _channels += 4;
  if (address_pin_4.has_value())
    _channels += 8;
  _channels += 1; // address pin 1 is mandatory

  _i2c_channels.resize(_channels + 1);
  for (uint8_t channel = 0; channel < _channels; channel++) {
    _i2c_channels[channel] = std::make_shared<I2cMultiplexerChannel>(i2c, channel, *this);
  }
  select_channel(0);
}

Status
I2cMultiplexerGpioID::select_channel(uint8_t channel)
{
  if (channel > _channels)
    return Status::Invalid("Channel out of range");
  if (channel == _selected_channel)
    return Status::OK();
  _address_pin_1->write((channel & 0x01) ? 1 : 0);
  if (_address_pin_2.has_value())
    (*_address_pin_2)->write((channel & 0x02) ? 1 : 0);
  if (_address_pin_3.has_value())
    (*_address_pin_3)->write((channel & 0x04) ? 1 : 0);
  if (_address_pin_4.has_value())
    (*_address_pin_4)->write((channel & 0x08) ? 1 : 0);
  _selected_channel = channel;
  TickerBase::get_instance().delay_nop(_switch_delay_us);
  return Status::OK();
}

uint8_t
I2cMultiplexerGpioID::get_selected_channel() const
{
  return _selected_channel;
}

uint8_t
I2cMultiplexerGpioID::get_total_channels() const
{
  return _channels;
}

Result<std::shared_ptr<I2cBase>>
I2cMultiplexerGpioID::get_i2c_interface_for_channel(uint8_t channel)
{
  if (channel >= _channels)
    return Status::Invalid("Channel out of range");
  return Result<std::shared_ptr<I2cBase>>::OK(std::move(_i2c_channels[channel]));
}
