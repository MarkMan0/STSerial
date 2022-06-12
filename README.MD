# STSerial
**FOR FREERTOS IMPLEMENTATION SEE `freertos` BRANCH**

The goal of this project was to create a wrapper around the HAL UART library, to make working with the uart easier and more convenient. The project is done in platformio, but it uses the ST HAL library, and not Arduino.
UART reception is done using DMA, and then into an intermediate buffer, from where the user can read it.
UART transmission can be done in intermediate mode, where the messages are sent out using `HAL_UART_Transmit`. Or in buffered mode, where messages are put into a buffer, and sent out using the DMA, when `tick()`, or `flush()` is called. `printf` style formatting is also supported.
There are unit tests implemented for various parts fo the project.

This project is not intended to be used on it's own, or be extended. This is sort of a Demo and practical example on using the `UART_DMA` class and `pin_api`. My expectation is, that future users of this code will simply copy-paste the files into their own project.

## pin_api
This is a very lightweight wrapper around the GPIO functionality of HAL. the user can use the pins of the MCU in an Arduino-like way. The pins can be initialized using `pin_mode`, and set and read from using the corresponding functions.

## UART_DMA details
The code is relatively simple, and documented using doxygen. There were some design decisions, that will be explained here.

### Callbacks
The constructor of `UART_DMA` accepts 2 callbacks, which are for initializing the hardware and the interrupts of the uart interface. The project was tested on an *STM32F303K8*, implementation of both these functions is provided in the code.

To create more implementations, these functions should be created/defined for the new UART:
```
  static void uart2_hw_init(UART_DMA&);
  static void uart2_enable_isrs(UART_DMA&);
  static void uart2_rx_event_cb(UART_HandleTypeDef* huart, uint16_t Pos);
```

### ISR handling
The projects/examples I have looked used custom callbacks inside of UART, DMA and even Systick ISRs. The HAL library can handle all this on it's own. If you look at the [stm32f3xx_it.cpp](src/stm32f3xx_it.cpp) file, in the handlers, only HAL is called.
This functionality is achieved by setting `ReceptionType` to `HAL_UART_RECEPTION_TOIDLE`. This way, IDLE interrupt is correctly, and our callback is called both when the DMA buffer is full, and when the UART goes to idle.

We are using HAL callbacks for UART, which greatly simplifies the callback code. Inside the `hw_init` function, a callback is registered for the uart using `HAL_UART_RegisterRxEventCallback`. This callback will be called when IDLE or DMA_TxCplt event occurs. To see the implementation, go to [uart_msp_impl.cpp](src/uart_msp_impl.cpp).

### printf
When using `printf` method of the UART, the result is directly put into the transmission buffer. When this cannot be done, the function will not do anything. This can fail, if there is not enough continuous space in the transmission buffer. In this case, the transmission buffer should be reset by the userm so the head of the buffer is further away from the end of the buffer.

## Future work
+ printf can fail, when the head index is close to the end of the buffer, even when the buffer is empty. In this case, the buffer should be "rotated", so contents are preserved, but the free space is continuous.
+ I did my best to make an easily readable and usable abstraction ove HAL, but in parts it's a bit messy, especially the callbacks.
+ Make this a downloadable platformio library
