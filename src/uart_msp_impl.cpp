#include "main.h"
#include "pin_api.h"
#include "uart.h"

void UART_DMA::uart2_enable_isrs(UART_DMA& uart) {
  // enable IDLE interrupt
  SET_BIT(USART2->CR1, USART_CR1_IDLEIE);

  // TX DMA
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

  // RX DMA
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);

  HAL_NVIC_SetPriority(USART2_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
}

void UART_DMA::uart2_hw_init(UART_DMA& uart) {
  uart.huart_.Instance = USART2;
  uart.huart_.Init.BaudRate = uart.baudrate_;
  uart.huart_.Init.WordLength = UART_WORDLENGTH_8B;
  uart.huart_.Init.StopBits = UART_STOPBITS_1;
  uart.huart_.Init.Parity = UART_PARITY_NONE;
  uart.huart_.Init.Mode = UART_MODE_TX_RX;
  uart.huart_.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uart.huart_.Init.OverSampling = UART_OVERSAMPLING_16;
  uart.huart_.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;

  uart.huart_.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;


  HAL_UART_RegisterCallback(&uart.huart_, HAL_UART_MSPINIT_CB_ID, UART_DMA::uart2_msp_init);
  if (HAL_UART_Init(&uart.huart_) != HAL_OK) {
    while (1) {
    }
  }

  HAL_UART_RegisterRxEventCallback(&uart.huart_, UART_DMA::uart2_rx_event_cb);
}

void UART_DMA::uart2_msp_init(UART_HandleTypeDef* uart) {
  if (uart->Instance == USART2) {
    __HAL_RCC_USART2_CLK_ENABLE();

    pin_mode(pins::rx, pin_mode_t::ALTERNATE_PP, GPIO_AF7_USART2);
    pin_mode(pins::tx, pin_mode_t::ALTERNATE_PP, GPIO_AF7_USART2);

    __HAL_RCC_DMA1_CLK_ENABLE();

    uart2.hdmatx_.Instance = DMA1_Channel7;
    uart2.hdmatx_.Init.Direction = DMA_MEMORY_TO_PERIPH;
    uart2.hdmatx_.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    uart2.hdmatx_.Init.MemInc = DMA_MINC_ENABLE;
    uart2.hdmatx_.Init.Mode = DMA_NORMAL;
    uart2.hdmatx_.Init.PeriphDataAlignment = DMA_MDATAALIGN_BYTE;
    uart2.hdmatx_.Init.PeriphInc = DMA_PINC_DISABLE;
    uart2.hdmatx_.Init.Priority = DMA_PRIORITY_LOW;

    if (HAL_DMA_Init(&uart2.hdmatx_) != HAL_OK) {
      // uart2.transmit("HAL DMA Init failed for tx");
    }

    uart2.hdmarx_.Instance = DMA1_Channel6;
    uart2.hdmarx_.Init.Direction = DMA_PERIPH_TO_MEMORY;
    uart2.hdmarx_.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    uart2.hdmarx_.Init.MemInc = DMA_MINC_ENABLE;
    uart2.hdmarx_.Init.Mode = DMA_CIRCULAR;
    uart2.hdmarx_.Init.PeriphDataAlignment = DMA_MDATAALIGN_BYTE;
    uart2.hdmarx_.Init.PeriphInc = DMA_PINC_DISABLE;
    uart2.hdmarx_.Init.Priority = DMA_PRIORITY_LOW;

    if (HAL_DMA_Init(&uart2.hdmarx_) != HAL_OK) {
      // uart2.transmit("HAL DMA Init failed for rx");
    }

    __HAL_LINKDMA(uart, hdmatx, uart2.hdmatx_);
    __HAL_LINKDMA(uart, hdmarx, uart2.hdmarx_);
  }
}

void UART_DMA::uart2_rx_event_cb(UART_HandleTypeDef* huart, uint16_t Pos) {
  static uint16_t last_pos = 0;
  for (; last_pos < Pos; ++last_pos) {
    uart2.receive_buff_.push(uart2.dma_buff_[last_pos]);
  }
  last_pos = last_pos % uart2.dma_buff_.size();
}



void UART_DMA::uart1_enable_isrs(UART_DMA& uart) {
  // enable IDLE interrupt
  SET_BIT(USART1->CR1, USART_CR1_IDLEIE);

  // TX DMA
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);

  // RX DMA
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

  HAL_NVIC_SetPriority(USART1_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
}

void UART_DMA::uart1_hw_init(UART_DMA& uart) {
  uart.huart_.Instance = USART1;
  uart.huart_.Init.BaudRate = uart.baudrate_;
  uart.huart_.Init.WordLength = UART_WORDLENGTH_8B;
  uart.huart_.Init.StopBits = UART_STOPBITS_1;
  uart.huart_.Init.Parity = UART_PARITY_NONE;
  uart.huart_.Init.Mode = UART_MODE_TX_RX;
  uart.huart_.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uart.huart_.Init.OverSampling = UART_OVERSAMPLING_16;
  uart.huart_.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;

  uart.huart_.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;


  HAL_UART_RegisterCallback(&uart.huart_, HAL_UART_MSPINIT_CB_ID, UART_DMA::uart1_msp_init);
  if (HAL_UART_Init(&uart.huart_) != HAL_OK) {
    while (1) {
    }
  }

  HAL_UART_RegisterRxEventCallback(&uart.huart_, UART_DMA::uart1_rx_event_cb);
}

void UART_DMA::uart1_msp_init(UART_HandleTypeDef* uart) {
  if (uart->Instance == USART1) {
    __HAL_RCC_USART1_CLK_ENABLE();

    pin_mode(pins::rx1, pin_mode_t::ALTERNATE_PP, GPIO_AF7_USART2);
    pin_mode(pins::tx1, pin_mode_t::ALTERNATE_PP, GPIO_AF7_USART2);

    __HAL_RCC_DMA1_CLK_ENABLE();

    uart1.hdmatx_.Instance = DMA1_Channel4;
    uart1.hdmatx_.Init.Direction = DMA_MEMORY_TO_PERIPH;
    uart1.hdmatx_.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    uart1.hdmatx_.Init.MemInc = DMA_MINC_ENABLE;
    uart1.hdmatx_.Init.Mode = DMA_NORMAL;
    uart1.hdmatx_.Init.PeriphDataAlignment = DMA_MDATAALIGN_BYTE;
    uart1.hdmatx_.Init.PeriphInc = DMA_PINC_DISABLE;
    uart1.hdmatx_.Init.Priority = DMA_PRIORITY_LOW;

    if (HAL_DMA_Init(&uart1.hdmatx_) != HAL_OK) {
      // uart2.transmit("HAL DMA Init failed for tx");
    }

    uart1.hdmarx_.Instance = DMA1_Channel5;
    uart1.hdmarx_.Init.Direction = DMA_PERIPH_TO_MEMORY;
    uart1.hdmarx_.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    uart1.hdmarx_.Init.MemInc = DMA_MINC_ENABLE;
    uart1.hdmarx_.Init.Mode = DMA_CIRCULAR;
    uart1.hdmarx_.Init.PeriphDataAlignment = DMA_MDATAALIGN_BYTE;
    uart1.hdmarx_.Init.PeriphInc = DMA_PINC_DISABLE;
    uart1.hdmarx_.Init.Priority = DMA_PRIORITY_LOW;

    if (HAL_DMA_Init(&uart1.hdmarx_) != HAL_OK) {
      // uart2.transmit("HAL DMA Init failed for rx");
    }

    __HAL_LINKDMA(uart, hdmatx, uart1.hdmatx_);
    __HAL_LINKDMA(uart, hdmarx, uart1.hdmarx_);
  }
}

void UART_DMA::uart1_rx_event_cb(UART_HandleTypeDef* huart, uint16_t Pos) {
  static uint16_t last_pos = 0;
  for (; last_pos < Pos; ++last_pos) {
    uart1.receive_buff_.push(uart1.dma_buff_[last_pos]);
  }
  last_pos = last_pos % uart1.dma_buff_.size();
}
