#include "Timing.hpp"
#include "stmepic.hpp"
#include "status.hpp"
#include <cstdint>
#include <memory>
#include <string>


using namespace stmepic;

uint32_t stmepic::frequency_to_period_us(float frequency) {
  return (uint32_t)(1000000.0f / frequency);
}


void TickerBase::irq_update_ticker() {
  tick_millis++;
  tick_micros = tick_millis * 1000;
}

TickerBase &TickerBase::get_instance() {
  static TickerBase *ticker;
  if(ticker == nullptr) {
    ticker = new TickerBase();
  }
  return *ticker;
}


uint32_t TickerBase::get_micros() {
  if(timer == nullptr)
    return 0;
  vPortEnterCritical();
  uint32_t mic = (uint32_t)timer->Instance->CNT + tick_micros;
  vPortExitCritical();
  return mic;
}

uint32_t TickerBase::get_millis() const {
  return tick_millis;
}

float TickerBase::get_seconds() {
  return (float)get_micros() * 0.000001f;
}

void TickerBase::delay(uint32_t miliseconds) {
  uint32_t start = get_micros();
  while(get_micros() - start < miliseconds) {
  }
}

void TickerBase::delay_nop(uint32_t microseconds) {
  // whats this magic K number?
  // this is the number of cycles that the CPU will run in 1us
  // why K=12 000 000 one might ask?
  // because the CPU is at some Fk frequency when divided by 1000000 we get the number of cycles in fill 1us
  // then the number 12 is approximate number of cycles that the CPU will run to iterate the through the loop.
  // so then K = 1000000 * 12 = 12000
  microseconds *= HAL_RCC_GetHCLKFreq() / 12000000; // magic K number
  for(uint32_t i = 0; i < microseconds; i++) {
    __NOP();
  }
}

void Timer::set_behaviour(uint32_t _period, bool _repeat) {
  period = _period;
  repeat = _repeat;
}

Timer::Timer(TickerBase &_ticker) : ticker(_ticker) {
  period         = 0;
  last_time      = ticker.get_micros();
  repeat         = true;
  timer_enabled  = true;
  function       = nullptr;
  triggered_flag = false;
}

Result<std::shared_ptr<Timer>> Timer::Make(uint32_t period, bool repeat, callback_funciton function, TickerBase &ticker) {
  auto new_timer = new Timer(ticker);
  new_timer->set_behaviour(period, repeat);
  new_timer->function = function;
  auto timer          = std::shared_ptr<Timer>(new_timer);
  return Result<decltype(timer)>::OK(std::move(timer));
}

void Timer::timer_reset() {
  this->last_time      = ticker.get_micros() - 1001;
  this->triggered_flag = false;
}

void Timer::enable(bool timer_enabled) {
  this->timer_enabled = timer_enabled;
}

bool Timer::triggered() {
  uint32_t dif;
  uint32_t current_time = ticker.get_micros();

  if(!timer_enabled) {
    this->last_time = current_time;
    return false;
  }
  // why this is here?
  // because some times last_value is higher than the current_time why is that?
  // because the timer have  irq problems when the vale of the time is rapidly checked
  // which means that the timer has overflowed and the difference is gretaer than the period
  dif = current_time > this->last_time ? current_time - this->last_time : this->last_time - current_time;
  if(dif < this->period)
    return false;
  if(!repeat && triggered_flag)
    return false;
  this->triggered_flag = true;
  this->last_time      = current_time;

  return true;
}

void Timer::run_function() {
  if(!triggered())
    return;
  if(this->function == nullptr)
    return;
  this->function(*this);
}
