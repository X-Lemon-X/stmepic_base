#include "Timing.hpp"
#include "status.hpp"
#include "stmepic.hpp"
#include <cstdint>
#include <memory>
#include <string>

using namespace stmepic;

TickerBase* TickerBase::ticker = nullptr;

uint32_t
stmepic::frequency_to_period_us(float frequency)
{
  return (uint32_t)(1000000.0f / frequency);
}

void
TickerBase::irq_update_ticker()
{
  tick_millis++;
  tick_micros = tick_millis * 1000;
}

Status
TickerBase::init(TickerBase* _ticker)
{
  if (!_ticker) {
    return Status::Invalid("ticker is null pointer");
  }
  ticker = _ticker;
  return Status::OK();
}

TickerBase&
TickerBase::get_instance()
{
  return *ticker;
}

uint32_t
TickerBase::get_micros()
{
  return ticker->get_micros();
}

uint32_t
TickerBase::get_millis() const
{
  return tick_millis;
}

float
TickerBase::get_seconds()
{
  return (float)get_micros() * 0.000001f;
}

void
TickerBase::delay(uint32_t miliseconds)
{
  uint32_t start = get_micros();
  while (get_micros() - start < miliseconds) {
  }
}

void
TickerBase::delay_nop(uint32_t iterations)
{
  for (uint32_t i = 0; i < iterations; ++i) {
#if defined(__xtensa__) || defined(__riscv) || defined(__ARM_ARCH)
    // Works for ESP32 (Xtensa/RISC-V) and STM32 (ARM)
    asm volatile("nop");
#else
    // Fallback for standard x86/desktop testing
    asm volatile("");
#endif
  }
}

void
Timer::set_behaviour(uint32_t _period, bool _repeat)
{
  period = _period;
  repeat = _repeat;
}

Timer::Timer(TickerBase& _ticker)
  : ticker(_ticker)
{
  period = 0;
  last_time = ticker.get_micros();
  repeat = true;
  timer_enabled = true;
  function = nullptr;
  triggered_flag = false;
}

Result<std::shared_ptr<Timer>>
Timer::Make(uint32_t period, bool repeat, callback_funciton function, TickerBase& ticker)
{
  auto new_timer = new Timer(ticker);
  new_timer->set_behaviour(period, repeat);
  new_timer->function = function;
  auto timer = std::shared_ptr<Timer>(new_timer);
  return Result<decltype(timer)>::OK(std::move(timer));
}

void
Timer::timer_reset()
{
  this->last_time = ticker.get_micros() - 1001;
  this->triggered_flag = false;
}

void
Timer::enable(bool timer_enabled)
{
  this->timer_enabled = timer_enabled;
}

bool
Timer::triggered()
{
  uint32_t dif;
  uint32_t current_time = ticker.get_micros();

  if (!timer_enabled) {
    this->last_time = current_time;
    return false;
  }
  // why this is here?
  // because some times last_value is higher than the current_time why is that?
  // because the timer have  irq problems when the vale of the time is rapidly checked
  // which means that the timer has overflowed and the difference is gretaer than the
  // period
  dif = current_time > this->last_time ? current_time - this->last_time
                                       : this->last_time - current_time;
  if (dif < this->period)
    return false;
  if (!repeat && triggered_flag)
    return false;
  this->triggered_flag = true;
  this->last_time = current_time;

  return true;
}

void
Timer::run_function()
{
  if (!triggered())
    return;
  if (this->function == nullptr)
    return;
  this->function(*this);
}
