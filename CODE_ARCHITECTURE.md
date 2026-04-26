# AM32 工程代码架构指南

## 1. 目录结构

```
AM32/
├── Src/                          # 公共代码 (所有 MCU 共享)
│   ├── main.c                    # 入口函数
│   ├── functions.c               # 电机控制核心算法
│   ├── signal.c                  # 输入信号处理
│   ├── dshot.c                  # DShot 协议
│   └── ...
├── Inc/                          # 公共头文件
│   ├── main.h
│   ├── targets.h                 # 硬件定义 (引脚/定时器映射)
│   ├── functions.h
│   └── ...
├── Mcu/
│   └── f103/                     # GD32F103 专用代码
│       ├── Src/
│       │   ├── peripherals.c     # 外设初始化
│       │   ├── comparator.c      # 比较器 BEMF
│       │   ├── ADC.c             # ADC 配置
│       │   ├── IO.c             # DShot 输入捕获
│       │   ├── phaseouts.c      # 相输出
│       │   └── ...
│       ├── Inc/
│       │   ├── peripherals.h    # 外设函数声明
│       │   ├── comparator.h
│       │   └── ...
│       ├── Drivers/              # GD32 官方固件库 (现已直接复制)
│       │   ├── CMSIS/           # ARM Cortex-M 核心头文件
│       │   └── GD32F10x_standard_peripheral/  # 外设驱动
│       │       └── Source/      # .c 驱动源文件
│       └── Startup/
│           └── startup_gd32f10x_md.s  # 启动代码
├── Makefile                      # 主 Makefile
└── f103makefile.mk              # F103 构建配置
```

---

## 2. 代码调用流程

### 2.1 启动流程

```
main.c main()
    │
    ├─► initAfterJump()              // 初始化 Flash/中断
    │
    ├─► initCorePeripherals()        // 初始化外设
    │       │
    │       ├─► MX_GPIO_Init()       // GPIO 配置
    │       ├─► MX_DMA_Init()        // DMA 配置
    │       ├─► TIM0_Init()          // TIMER0 PWM
    │       ├─► TIMER2_Init()        // DShot 输入
    │       ├─► TIMER4_Init()        // 间隔定时器
    │       ├─► TIMER5_Init()        // 通用定时器
    │       ├─► TIMER6_Init()        // 10kHz 控制循环
    │       ├─► TIMER13_Init()       // 换相定时器
    │       └─► COMP_Init()          // 比较器 (空函数)
    │
    ├─► enableCorePeripherals()      // 使能外设
    │       │
    │       ├─► TIMER0 PWM 输出使能
    │       ├─► TIMER2 输入使能
    │       └─► 启动各定时器
    │
    └─► loadEEpromSettings()         // 加载 EEPROM 设置
```

### 2.2 电机控制流程

```
信号输入 (DShot/PWM)
    │
    ▼
signal.c 解析信号
    │
    ▼
functions.c 电机控制
    │
    ├─► commutate()              // 换相逻辑
    │       │
    │       └─► setPwmPhases()  // 设置 PWM 占空比
    │               │
    │               └─► TIMER0_CH0/1/2 CV 寄存器
    │
    ├─► 读取比较器 BEMF
    │       └─► comparator.c getCompOutputLevel()
    │
    └─► ADC 采样 (电流/电压)
            └─► ADC.c getCurrentAdcValue()
```

---

## 3. 头文件包含关系

### 3.1 main.c 的 include 链

```c
#include "main.h"              // Inc/main.h
    │
    ├─► targets.h            // 硬件定义 (HARDWARE_GROUP_F103)
    │
    └─► gd32f10x.h           // GD32 芯片头文件
            │
            ├─► gd32f10x_rcu.h     // RCU 时钟
            ├─► gd32f10x_gpio.h    // GPIO
            ├─► gd32f10x_timer.h   // TIMER
            ├─► gd32f10x_adc.h     // ADC
            ├─► gd32f10x_dma.h     // DMA
            ├─► gd32f10x_exti.h    // EXTI
            └─► ...                 // 等等

#include "peripherals.h"      // Mcu/f103/Inc/peripherals.h
    │
    └─► main.h               // 循环包含

#include "ADC.h"              // Mcu/f103/Inc/ADC.h
#include "IO.h"               // Mcu/f103/Inc/IO.h
#include "comparator.h"       // Mcu/f103/Inc/comparator.h
```

### 3.2 GD32 标准库包含

```
gd32f10x.h (Mcu/f103/Drivers/CMSIS/GD/GD32F10x/Include/)
    │
    ├─► core_cm3.h           // ARM CMSIS 核心
    ├─► system_gd32f10x.h    // 系统时钟
    └─► (其他芯片相关头文件)

应用代码
    │
    └─► #include "gd32f10x_xxx.h"
            │
            └─► gd32f10x_xxx.c (在 Drivers/GD32F10x_standard_peripheral/Source/)
```

---

## 4. 编译链接机制

### 4.1 Makefile 源文件收集

```makefile
# 1. 公共代码 (所有 MCU)
SRC_COMMON := $(foreach dir,$(SRC_DIRS_COMMON),$(wildcard $(dir)/*.[cs]))

# 2. MCU 特定代码
SRC_DIR_$(MCU) := $(SRC_BASE_DIR_$(MCU)) $(HAL_FOLDER_$(MCU))/Src
SRC_$(MCU) := $(foreach dir,$(SRC_DIR_$(MCU)),$(wildcard $(dir)/*.[cs]))

# 3. 最终编译命令
$(QUIET)$(xCC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRC_COMMON) $(SRC_$(MCU))
```

### 4.2 F103 源文件目录

| 目录 | 内容 | 作用 |
|------|------|------|
| `Src/` | main.c, functions.c | 公共业务逻辑 |
| `Mcu/f103/Src/` | peripherals.c, ADC.c | GD32F103 外设驱动 |
| `Mcu/f103/Drivers/` | CMSIS/, standard_peripheral/ | 官方固件库 |

### 4.3 编译顺序

```
1. 编译 Src/*.c       → 目标文件 (.o)
2. 编译 Mcu/f103/Src/*.c  → 目标文件 (.o)
3. 编译 Drivers/GD32F10x_standard_peripheral/Source/*.c → 目标文件 (.o)
4. 链接所有 .o 文件   → ELF 文件
```

---

## 5. 关键函数映射

### 5.1 main.c 调用链

| main.c 调用 | 实际定义 | 文件 |
|-------------|----------|------|
| `initCorePeripherals()` | `initCorePeripherals()` | peripherals.c |
| `enableCorePeripherals()` | `enableCorePeripherals()` | peripherals.c |
| `COMP_Init()` | `COMP_Init()` | peripherals.c |
| `setPWMCompare1/2/3()` | `setPWMCompare1/2/3()` | peripherals.c |
| `generatePwmTimerEvent()` | `generatePwmTimerEvent()` | peripherals.c |
| `commutate()` | `commutate()` | functions.c |
| `getCompOutputLevel()` | `getCompOutputLevel()` | comparator.c |

### 5.2 GD32 标准库调用

| 应用代码调用 | 实际定义 | 文件 |
|-------------|----------|------|
| `gpio_init()` | `gpio_init()` | gd32f10x_gpio.c |
| `timer_init()` | `timer_init()` | gd32f10x_timer.c |
| `rcu_periph_clock_enable()` | `rcu_periph_clock_enable()` | gd32f10x_rcu.c |
| `adc_regular_channel_config()` | `adc_regular_channel_config()` | gd32f10x_adc.c |
| `dma_init()` | `dma_init()` | gd32f10x_dma.c |
| `exti_init()` | `exti_init()` | gd32f10x_exti.c |

---

## 6. targets.h 的作用

`Inc/targets.h` 是**硬件抽象层**，将通用名称映射到具体芯片：

```c
#ifdef HARDWARE_GROUP_F103
    // GPIO 引脚映射
    #define INPUT_PIN GPIO_PIN_0
    #define INPUT_PIN_PORT GPIOA

    // 定时器映射 (GD32F103 实际存在的定时器)
    #define UTILITY_TIMER TIMER5
    #define INTERVAL_TIMER TIMER4
    #define TEN_KHZ_TIMER TIMER6
    #define COM_TIMER TIMER13    // 注: 实际用 TIMER7

    // 比较器
    #define PHASE_A_COMP 0x01    // PB1
    #define PHASE_B_COMP 0x02    // PB2
    #define PHASE_C_COMP 0x0A    // PB10
#endif
```

这样 `peripherals.c` 中使用 `UTILITY_TIMER` 等宏，编译时会自动替换为 `TIMER5` 等具体值。

---

## 7. 数据流图

```
                    ┌──────────────┐
                    │   main.c     │
                    │  main()      │
                    └──────┬───────┘
                           │
            ┌───────────────┼───────────────┐
            │               │               │
            ▼               ▼               ▼
     ┌──────────┐   ┌───────────┐   ┌──────────┐
     │peripherals│   │functions  │   │ targets.h│
     │   .c     │   │   .c      │   │  (宏)    │
     └────┬─────┘   └─────┬─────┘   └──────────┘
          │                │
          │        ┌───────┴───────┐
          │        │               │
          ▼        ▼               ▼
   ┌───────────┐ ┌─────────┐ ┌──────────┐
   │gd32f10x_  │ │compar-  │ │  ADC.c   │
   │gpio.c     │ │ator.c   │ │          │
   │timer.c    │ │         │ │          │
   │rcu.c      │ │         │ │          │
   │adc.c      │ │         │ │          │
   └───────────┘ └─────────┘ └──────────┘
          │               │               │
          └───────────────┼───────────────┘
                          │
                          ▼
              ┌─────────────────────┐
              │ GD32F10x_standard_ │
              │ peripheral/Source   │
              │ (gd32f10x_*.c)      │
              └─────────────────────┘
```

---

## 8. 调试建议

### 查看函数定义位置
```bash
# 在整个项目中搜索
grep -r "void TIM0_Init" AM32/

# 查看符号表
arm-none-eabi-nm obj/AM32_GD32F103_F103_2.20.elf | grep TIM0_Init
```

### 查看编译依赖
```bash
# 生成 .d 依赖文件
make f103 -MMD

# 查看某文件的依赖
cat obj/*.d | grep peripherals.c
```

---

## 9. 快速参考

| 问题 | 查看文件 |
|------|----------|
| GPIO 引脚定义 | `Inc/targets.h` (HARDWARE_GROUP_F103) |
| 定时器配置 | `Mcu/f103/Src/peripherals.c` |
| ADC 配置 | `Mcu/f103/Src/ADC.c` |
| 比较器 BEMF | `Mcu/f103/Src/comparator.c` |
| 电机换相逻辑 | `Src/functions.c` |
| 主循环 | `Src/main.c` |
| DShot 输入 | `Mcu/f103/Src/IO.c` |
| 官方固件库 | `Mcu/f103/Drivers/GD32F10x_standard_peripheral/` |
