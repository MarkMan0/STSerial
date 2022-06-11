#include "uart.h"
#include "pin_api.h"


UART_DMA::UART_DMA(hw_init_fcn_t* a, isr_enable_fcn_t* b) : hw_init_cb(a), isr_enable_cb(b) {
}

void UART_DMA::begin(uint32_t baud) {
  baudrate_ = baud;
  hw_init_cb(*this);
  isr_enable_cb(*this);

  HAL_UART_Receive_DMA(&huart_, dma_buff_.data(), dma_buff_.size());
  huart_.ReceptionType = HAL_UART_RECEPTION_TOIDLE;  // also reacts to IDLE interrupt, so only one callback is needed

  // enable IDLE interrupt
  SET_BIT(huart_.Instance->CR1, USART_CR1_IDLEIE);
}

void UART_DMA::send(const void* buff, size_t sz) {
  size_t sent = 0;
  while (sent < sz) {
    sent += transmit_buff_.push(reinterpret_cast<const uint8_t*>(buff) + sent, sz - sent);
  }
}

void UART_DMA::flush() {
  while (uint16_t n = transmit_buff_.get_occupied_continuous()) {
    HAL_UART_Transmit_DMA(&huart_, const_cast<uint8_t*>(&transmit_buff_.peek()), n);
    transmit_buff_.pop(n);
  }
}

void UART_DMA::tick() {
  flush();
}


uint16_t UART_DMA::vprintf(const char* fmt, va_list args) {
  auto msglen = vsnprintf(nullptr, 0, fmt, args);

  auto ptr = transmit_buff_.reserve(msglen + 1);
  if (!ptr) {
    return 0;
  }

  return vsnprintf(reinterpret_cast<char*>(ptr), msglen, fmt, args);
}
