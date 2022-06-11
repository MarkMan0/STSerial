#pragma once

#include "main.h"
#include "ring_buffer.h"
#include <array>
#include <cstring>


/**
 * @brief Uart wrapper with DMA for rx and tx
 * @details In theory, it can be used for multiple uarts if set up properly in the constructor
 * User would need to implement the 4 functions for the constructor
 * Implementation is provided for UART2
 *
 */
class UART_DMA {
public:
  using hw_init_fcn_t = void(UART_DMA&);
  using isr_enable_fcn_t = void(UART_DMA&);

  UART_DMA(hw_init_fcn_t*, isr_enable_fcn_t*);

  void begin(uint32_t baud);

  uint16_t available() const {
    return receive_buff_.get_num_occupied();
  }

  uint8_t get_one() {
    return receive_buff_.pop();
  }

  /** @name Immediate transmission
   *  @details Messages are sent out immediately
   */
  ///@{
  void transmit(uint8_t c) {
    HAL_UART_Transmit(&huart_, &c, 1, HAL_MAX_DELAY);
  }
  void transmit(const char* str, size_t n) {
    HAL_UART_Transmit(&huart_, reinterpret_cast<uint8_t*>(const_cast<char*>(str)), n, HAL_MAX_DELAY);
  }
  void transmit(const char* str) {
    transmit(str, strlen(str));
  }
  ///@}

  /** @name transmit messages using buffer in tick()
   *  @details Messages are placed into transmission_buffer and sent out later, asynchronously
   */
  ///@{
  void send(const char*, size_t);
  void send(const char* str) {
    send(str, strlen(str));
  }

  /// @brief Flush the transmission buffer
  void flush();
  ///@}

  void tick();  //!< Called peridically to empty the transmit buffer

  UART_HandleTypeDef huart_;
  DMA_HandleTypeDef hdmarx_, hdmatx_;

private:
  uint32_t baudrate_{ 115200 };
  uint16_t last_rxdma_pos_{ 0 };           ///< Used in rx event callback to track DMA
  std::array<uint8_t, 64> dma_buff_;       ///< Buffer sued by the DMA to receive
  RingBuffer<uint8_t, 64> receive_buff_;   ///< Messages from DMA buffer are put here
  RingBuffer<uint8_t, 64> transmit_buff_;  ///< Buffer for non-immediate transmission

  const hw_init_fcn_t* hw_init_cb;
  const isr_enable_fcn_t* isr_enable_cb;


public:
  /** @brief Generic RX event callback */
  void rx_event_cb(UART_HandleTypeDef*, uint16_t);

  /** @name UART2 init and callback functions */
  ///@{

  /** @brief Initialization of the UART peripherial and handle */
  static void uart2_hw_init(UART_DMA&);
  /** @brief Enables interrupts for UART and DMAs */
  static void uart2_enable_isrs(UART_DMA&);
  /** @brief Called by HAL on DMA or IDLE interrupt */
  static void uart2_rx_event_cb(UART_HandleTypeDef* huart, uint16_t Pos);

  ///@}


  /** @name UART1 init and callback functions */
  ///@{

  /** @brief Initialization of the UART peripherial and handle */
  static void uart1_hw_init(UART_DMA&);
  /** @brief Enables interrupts for UART and DMAs */
  static void uart1_enable_isrs(UART_DMA&);
  /** @brief Called by HAL on DMA or IDLE interrupt */
  static void uart1_rx_event_cb(UART_HandleTypeDef* huart, uint16_t Pos);

  ///@}
};

extern UART_DMA uart2, uart1;
