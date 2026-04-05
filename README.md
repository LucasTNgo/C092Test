# C092Test
(README generated using Claude)

A test bench project for the STM32C092 microcontroller (NUCLEO-C092RC), 
primarily focused on validating CANopen communication, peripheral drivers, 
and sensor integration ahead of a production outdoor deployment.

## Hardware

- **Board:** NUCLEO-C092RC
- **MCU:** STM32C092RC (Cortex-M0+, 256KB Flash, 30KB RAM)
- **CAN Transceiver:** External transceiver required (e.g. TJA1051, SN65HVD230)
- **Clock:** 48MHz HSE (Any HSE crystal recommended for production)

## Software Stack

- **CANopenNode** v4.0 (pinned to commit `8c7d852`)
- **CANopenSTM32** (pinned to commit `e647320`)
- **STM32 HAL** via STM32CubeMX
- **Build System:** CMake + Ninja via STM32Cube VS Code extension
- **Toolchain:** arm-none-eabi-gcc 14.3.1

## Project Structure
C092Test/
├── Core/                   # CubeMX generated application code
│   ├── Inc/
│   │   ├── main.h
│   │   ├── OD.h            # CANopen Object Dictionary
│   └── Src/
│       ├── main.c
│       ├── OD.c            # CANopen Object Dictionary
├── Libs/
│   ├── CANopenNode/        # CANopen protocol stack (submodule)
│   └── CANopenSTM32/       # STM32 CANopen driver (submodule)
├── cmake/
│   └── stm32cubemx/        # CubeMX generated CMake files
├── Drivers/                # STM32 HAL + CMSIS
└── CMakeLists.txt

## Getting Started

### Prerequisites

- [VS Code](https://code.visualstudio.com/) with STM32Cube extension
- [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html)
- arm-none-eabi-gcc (installed via STM32Cube extension)
- Git

### Clone
```bash
git clone --recurse-submodules https://github.com/LucasTNgo/C092Test.git
cd C092Test
```

If you forgot `--recurse-submodules`:
```bash
git submodule update --init
```

### Build

Open the project folder in VS Code, then:
Ctrl+Shift+P → CMake: Configure
Ctrl+Shift+P → CMake: Build

Or from the bottom status bar, click **Build**.

## CANopen Configuration

| Parameter | Value |
|-----------|-------|
| Node ID | 1 |
| Baud Rate | 125 kbps |
| Timer | TIM17 (1ms tick) |
| FDCAN | FDCAN1 |

## Test Objectives

- [ ] Verify CANopen bootup and heartbeat on NUCLEO-C092RC
- [ ] Confirm CAN connectivity with a second node or analyzer
- [ ] Validate FDCAN peripheral with integrated PHY on C092
- [ ] Test PDO transmission and reception
- [ ] Sensor integration and data transmission over PDO
- [ ] Validate RTC timekeeping

## License

This project integrates the following open source components:
- [CANopenNode](https://github.com/CANopenNode/CANopenNode) — Apache 2.0
- [CANopenSTM32](https://github.com/CANopenNode/CanOpenSTM32) — Apache 2.0
- STM32 HAL — BSD 3-Clause (ST Microelectronics)