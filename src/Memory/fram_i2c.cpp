
#include "fram_i2c.hpp"
#include "device.hpp"
#include "stmepic.hpp"
#include "status.hpp"

using namespace stmepic::memory;
using namespace stmepic;


Result<std::shared_ptr<FramI2C>>
FramI2C::Make(std::shared_ptr<I2cBase> _hi2c, uint8_t _device_address, uint16_t _begin_address, uint32_t _fram_size) {
  if(_hi2c == nullptr)
    return Status::Invalid("I2cBase is not initialized");
  if(_device_address == 0)
    return Status::Invalid("Device address is not valid");
  if(_begin_address > _fram_size)
    return Status::Invalid("Begin address is not valid");
  return Result<std::shared_ptr<FramI2C>>::OK(
  std::shared_ptr<FramI2C>(new FramI2C(_hi2c, _device_address, _begin_address, _fram_size)));
}

FramI2C::FramI2C(std::shared_ptr<I2cBase> _hi2c, uint8_t _device_address, uint16_t _begin_address, uint32_t _fram_size)
: hi2c(_hi2c), device_address(_device_address), begin_address(_begin_address), fram_size(_fram_size) {
}

FramI2C::~FramI2C() {
  device_stop();
}

Status FramI2C::device_get_status() {
  if(hi2c == nullptr)
    return Status::Invalid("I2cBase is not initialized");
  return hi2c->is_device_ready(device_address, 1, 100);
}

bool FramI2C::device_ok() {
  auto status = device_get_status();
  if(!status.ok())
    return false;
  return status.ok();
}

Result<bool> FramI2C::device_is_connected() {
  auto status = device_get_status();
  return status.ok() ? Result<bool>::OK(true) : status.status();
}

Status FramI2C::device_reset() {
  return device_get_status().status();
}

Status FramI2C::device_start() {
  return device_get_status().status();
}

Status FramI2C::device_stop() {
  return Status::OK();
}

Status FramI2C::device_set_settings(const DeviceSettings &settings) {
  (void)settings;
  return Status::OK();
}


Status FramI2C::write_raw(uint32_t address, uint8_t *data, size_t length) {
  return hi2c->write(device_address, begin_address + address, data, length);
}

Status FramI2C::read_raw(uint32_t address, uint8_t *data, size_t length) {
  return hi2c->read(device_address, begin_address + address, data, length);
}
