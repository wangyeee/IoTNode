# IoTNode

My IoT implememtation with STM32F0 and nRF24L01.

## Compile and run

1. Install [GNU Tools for ARM Embedded Processors](https://launchpad.net/gcc-arm-embedded). And execute `make` to compile the code.
2. For Linux and OS X, clone this [repo](https://github.com/texane/stlink), follow the instructions on that page and install st-util to your system.
3. Execute `make flash` to download the binray to a STM32F0 target via ST Link.
