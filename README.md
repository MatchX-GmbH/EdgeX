# EdgeX AI development firmware
This project is intended as a starting point for development of solutions based on EdgeX module. It consits of an implementation of LoRa stack, AI process, and all the necessary utilities and drivers for the hardware.

## Installation
### Dependencies
* SDK - https://github.com/kendryte/kendryte-standalone-sdk
* Toolchain and documentation - https://kendryte.com/downloads/
* KFlash GUI - https://github.com/sipeed/kflash_gui

### Steps
1. mkdir build; cd build;
2. cmake .. -DTOOLCHAIN="path/to/toolchain"
3. make

### Flashing
When the project is compiled, we can use KFlash GUI to upload it to flash memory. First, choose ai-node-fw.bin from ./build directory and specify target address to 0x0. Additionally, for human detection demo, you need to upload neural network. To do that, choose file ./src/tasks/ai/yolo.kmodel and specify the target address to be "0x800000". Board can be set to "Auto", "Burn To" parameter should be set to "Flash". Recommended baudrate is 3000000.

## Configuration
After connecting the Dev Kit to PC via usb cable, we can access the configuration and debug interface via UART. Henceforth, we can connect to the serial interface. For example, on Linux "minicom -D /dev/ttyUSB0"" could be used. The baudrate is 115200. There we can check and change the configuration of our module. All available commands can be checked by typing "help".
To configure our LoRa application and set Dev EUI, App EUI and Key we can use following commands:
* "set dev eui,01:02:03:04:05:06:07:08"
* "set app eui,01:02:03:04:05:06:07:08"
* "set app key,01:02:03:04:05:06:07:08:09:0A:0B:0C:0D:0E:0F:10"


## Source description
Currently the software runs without the aid of any OS. We run two main tasks independently on separate cores. The core logic for each task is present in corresponding subdirectory in /src/tasks. 
First task is related to AI functionality and had been based on [examples provided by Kendryte](https://github.com/kendryte/kendryte-standalone-demo).

Second task handles a LoRa stack. It has been sourced and adapted from [Semtech implementation](https://github.com/Lora-net/LoRaMac-node). The same repository was also used for radio driver and MAC layer implementation in /src/radio and /src/mac directories respectively. Dependencies on board utility functions have been implemented or stubbed in /src/radio/board.

## Debugging
OpenOCD can be used - reference: https://github.com/kendryte/openocd-kendryte

        
## Model development
Kendryte K210 KPU can run neural network models in .kmodel format. Conversion can be done from Tensorflow Lite fromat by using [nncase](https://github.com/kendryte/nncase). Guide on how to train a network for K210 can be found [here](https://github.com/kendryte/tensorflow-workspace).


## Recommended reading
* [Programming guide](https://github.com/kendryte/kendryte-doc-standalone-programming-guide)
* [Datasheet](https://github.com/kendryte/kendryte-doc-datasheet)
* [Demo source code](https://github.com/kendryte/kendryte-standalone-demo)
