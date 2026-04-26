# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

AM32 is an open-source BLDC (Brushless DC) motor controller firmware for ARM-based ESCs. This repository has been ported to GD32F103TBU6.

## Build Commands

```bash
make f103        # Build GD32F103 firmware
make clean       # Clean build artifacts
make targets     # List all available build targets
```

Output: `obj/AM32_GD32F103_F103_2.20.bin` (Flash: 13604 bytes, RAM: 3624 bytes)

## Architecture

### Directory Structure
- **Src/** - Common firmware code (motor control, DShot, telemetry)
- **Mcu/{mcu}/Src/** - MCU-specific peripheral implementations
- **Mcu/{mcu}/Inc/** - MCU-specific headers and register definitions
- **Mcu/{mcu}/Drivers/** - CMSIS and standard peripheral library
- **Mcu/{mcu}/Startup/** - Linker script and startup code

### Supported MCUs
E230, F031, F051, F415, F421, G071, G431, L431, V203, G031, F103

### Key Files
| File | Purpose |
|------|---------|
| `Src/main.c` | Entry point, main loop |
| `Src/functions.c` | Motor commutation, BEMF detection |
| `Src/dshot.c` | DShot protocol input |
| `Src/signal.c` | PWM/signal input processing |
| `Mcu/f103/Src/peripherals.c` | TIMER0 PWM, DMA, Timer init |
| `Mcu/f103/Src/comparator.c` | BEMF comparator (direct register access) |
| `Mcu/f103/Src/IO.c` | DShot input capture |
| `Inc/targets.h` | Hardware pin definitions, timer mappings |

## GD32F103 Porting Notes

### Critical Differences from Other MCUs
- **Comparator**: GD32F10x standard library has NO comparator driver. Uses direct register access to CMP_BASE=0x4001C000 (see `Mcu/f103/Src/comparator.c`)
- **TIMER13**: Does not exist in GD32F10x. Use TIMER7 instead (defined as COM_TIMER)
- **Device Type**: Must use `GD32F10X_HD` (not MD) for GD32F103TBU6
- **Flash Wait States**: 96MHz requires 2 wait states (`FMC_WSCNT_WSCNT_2`)

### Pin Mappings (HARDWARE_GROUP_F103)
- TIMER0 PWM: PA8/9/10 (CH0/1/2), PB13/14/15 (CH0N/1N/2N)
- DShot Input: PA0 (TIMER2 CH0)
- Comparator BEMF: PB1 (COMP1), PB2 (COMP2), PB10 (COMP3)
- LED: PB3

### Key Register Definitions
```c
#define CMP_BASE  0x4001C000
#define CMP       ((CMP_TypeDef *) CMP_BASE)
// CMP->CS bits: EN(0), SPEED(2-3), MSEL(4-6), OUTPUT(14)
```

## Troubleshooting

- **"bool redefinition" error**: Add `-D__bool_true_false_are__defined=1` to CFLAGS
- **"TIMERx_IRQn undeclared"**: Verify `GD32F10X_HD` is defined (not MD)
- **"ENET driver error"**: Exclude `gd32f10x_enet.c` in makefile
- **No comparator output**: GD32F103 requires direct register access, not std peripheral lib

## Documentation

- `PORTING_F103_NOTES.md` - Detailed GD32F103 porting documentation
- `README.md` - Original AM32 project documentation
