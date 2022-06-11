#include "uart.h"
#include "pin_api.h"


UART_DMA::UART_DMA(hw_init_fcn_t* a, isr_enable_fcn_t* b, msp_init_fcn_t* c, rx_event_fcn_t* d)
  : hw_init_cb(a), isr_enable_cb(b), msp_init_cb(c), rx_event_cb(d) {
}

void UART_DMA::begin(uint32_t baud) {
  baudrate_ = baud;
  hw_init_cb(*this);
  isr_enable_cb(*this);

  HAL_UART_Receive_DMA(&huart_, dma_buff_.data(), dma_buff_.size());
  huart_.ReceptionType = HAL_UART_RECEPTION_TOIDLE;  // also reacts to IDLE interrupt, so only one callback is needed
}

void UART_DMA::send(const char* buff, size_t sz) {
  size_t sent = 0;
  while (sent < sz) {
    sent += transmit_buff_.push(reinterpret_cast<const uint8_t*>(buff + sent), sz - sent);
  }
}

void UART_DMA::tick() {
  uint16_t n = transmit_buff_.get_occupied_continuous();
  if (n) {
    HAL_UART_Transmit_DMA(&huart_, const_cast<uint8_t*>(&transmit_buff_.peek()), n);
    transmit_buff_.pop(n);
  }
}



UART_DMA uart2(UART_DMA::uart2_hw_init, UART_DMA::uart2_enable_isrs, UART_DMA::uart2_msp_init,
               UART_DMA::uart2_rx_event_cb);

UART_DMA uart1(UART_DMA::uart1_hw_init, UART_DMA::uart1_enable_isrs, UART_DMA::uart1_msp_init,
               UART_DMA::uart1_rx_event_cb);
