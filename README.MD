# STSerial - Freertos

For more details, see Readme in master branch.

## Adapting for FreeRtos

The buffers are locked for simultaneous accessing using binary semaphores. To lock the binary semaphore, a `Lock` and a `LockISR` class is created in [rtos_utils.h](src/rtos_utils.h). These locks utilize RAII to unlock the mutex.

`tick()` is called in it's own task. This task waits for a notification. A notification is generated in `send` or `printf`, which will awaken this task.

In the receive callback, and optional task will be notified to handle the incoming data, without the need for checking periodically.

Tests had to be deleted for UART, until I figure out how to implement them using FreeRTOS in platformio.
