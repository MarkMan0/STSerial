/**
 * @file main.cpp
 * @brief Program entry point
 *
 */



#include "main.h"
#include "pin_api.h"
#include "uart.h"
#include <array>
void SystemClock_Config(void);


UART_DMA uart2(UART_DMA::uart2_hw_init, UART_DMA::uart2_enable_isrs);
UART_DMA uart1(UART_DMA::uart1_hw_init, UART_DMA::uart1_enable_isrs);


int main(void) {
  HAL_Init();
  SystemClock_Config();

  uart2.begin(115200);
  uart1.begin(115200);

  constexpr auto led_pin = PB3;
  pin_mode(led_pin, pin_mode_t::OUT_PP);
  std::array<char, 40> str;

  while (1) {
    if (uart2.available()) {
      str[0] = '\0';
      strncat(str.data(), "Got 2: \"", str.size() - 1);
      while (uart2.available()) {
        auto c = uart2.get_one();
        if (c == '\n' || c == '\r') {
          continue;
        }
        char s[] = { c, '\0' };
        strncat(str.data(), s, str.size() - 1);
      }
      strncat(str.data(), "\"", str.size() - 1);
      strncat(str.data(), "\n", str.size() - 1);

      uart1.send(str.data());
      uart2.send(str.data());
    }


    if (uart1.available()) {
      str[0] = '\0';
      strncat(str.data(), "Got 1: \"", str.size() - 1);
      while (uart1.available()) {
        auto c = uart1.get_one();
        if (c == '\n' || c == '\r') {
          continue;
        }
        char s[] = { c, '\0' };
        strncat(str.data(), s, str.size() - 1);
      }
      strncat(str.data(), "\"", str.size() - 1);
      strncat(str.data(), "\n", str.size() - 1);

      uart2.send(str.data());
    }

    uart1.tick();
    uart2.tick();


    toggle_pin(led_pin);
#ifdef NDEBUG
    HAL_Delay(500);
#else
    HAL_Delay(2000);
#endif
  }
}

void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
  RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
    Error_Handler();
  }
}


/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line) {
  while (1) {
    HAL_Delay(1000);
  }
}
#endif /* USE_FULL_ASSERT */


void HAL_MspInit(void) {
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
}
