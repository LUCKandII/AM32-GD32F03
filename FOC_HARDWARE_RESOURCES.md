# GD32F103 FOC 硬件资源预留说明

## 概述

本项目已为 FOC（磁场定向控制）预留完整的硬件资源，便于后续固件升级实现闭环矢量控制。

---

## 硬件资源映射

### ADC 通道配置 (8通道)

| ADC通道 | 引脚 | 功能 | 用途 |
|---------|------|------|------|
| ADC_CHANNEL_1 | PA1 | 电流采样 I_A | 三电阻法相电流检测 |
| ADC_CHANNEL_2 | PA2 | 电流采样 I_B | 三电阻法相电流检测 |
| ADC_CHANNEL_3 | PA3 | 电流采样 I_C | 三电阻法相电流检测 |
| ADC_CHANNEL_4 | PA4 | 相电压 V_A | 电机相电压采样 |
| ADC_CHANNEL_5 | PA5 | 相电压 V_B | 电机相电压采样 (FOC预留) |
| ADC_CHANNEL_6 | PA6 | 相电压 V_C | 电机相电压采样 (FOC预留) |
| ADC_CHANNEL_7 | PA7 | 母线电压 V_BUS | 直流母线电压采样 |
| ADC_CHANNEL_8 | PB0 | 温度传感器 | 电机/驱动器温度检测 |

**注意**: PA1/PA2/PA3 电流采样需外部运放进行信号调理。

### 比较器资源

| 比较器 | 引脚 | 用途 |
|--------|------|------|
| COMP1 | PB1 | Phase A BEMF 检测 |
| COMP2 | PB2 | Phase B BEMF 检测 |
| COMP3 | PB10 | Phase C BEMF 检测 |

**当前用途**: 六步换相 BEMF 过零检测
**FOC扩展**: 可用于过流保护或转子位置估算

### PWM 输出 (TIMER0)

| 通道 | 引脚 | 功能 |
|------|------|------|
| TIMER0_CH0 | PA8 | Phase A High |
| TIMER0_CH0N | PB13 | Phase A Low |
| TIMER0_CH1 | PA9 | Phase B High |
| TIMER0_CH1N | PB14 | Phase B Low |
| TIMER0_CH2 | PA10 | Phase C High |
| TIMER0_CH2N | PB15 | Phase C Low |

**配置**: 6路互补PWM，带死区插入，适合三相H桥驱动

---

## FOC 开发路线图

### 阶段1: 硬件验证
- [ ] 验证8通道ADC采样精度
- [ ] 校准电流采样通道 (PA1/PA2/PA3)
- [ ] 验证母线电压采样 (PA7)
- [ ] 测试比较器响应速度

### 阶段2: 开环 FOC
- [ ] 实现 Clark/Park 变换
- [ ] SVPWM 生成
- [ ] 开环转速控制

### 阶段3: 闭环 FOC
- [ ] 电流环 PID
- [ ] 速度环 PID
- [ ] 位置环 (可选)

---

## 关键代码位置

| 功能 | 文件 | 备注 |
|------|------|------|
| ADC初始化 | `Mcu/f103/Src/ADC.c` | 8通道配置 |
| GPIO配置 | `Mcu/f103/Src/peripherals.c` | MX_GPIO_Init() |
| 比较器 | `Mcu/f103/Src/comparator.c` | BEMF检测 |
| PWM输出 | `Mcu/f103/Src/peripherals.c` | TIMER0_Init() |
| 定时器映射 | `Inc/targets.h` | HARDWARE_GROUP_F103 |

---

## FOC 代码实现指南

### 1. 新建 FOC 相关文件

建议在 `Src/` 目录下创建:

```
Src/foc/
├── foc.c          # FOC 核心算法
├── foc.h          # 头文件
├── svpwm.c        # SVPWM 生成
├── svpwm.h        #
├── park.c         # Park/Clark 变换
├── park.h         #
├── pid.c          # PID 控制器
└── pid.h          #
```

### 2. ADC 采样时序配置

FOC 需要在 PWM 周期的特定时刻采样电流，需配置 ADC 触发源：

**修改文件**: `Mcu/f103/Src/peripherals.c`

```c
// TIMER0 输出比较触发 ADC (在 PWM 中心点采样)
void TIMER0_ADC_Trigger_Init(void)
{
    // 配置 TIMER0 CH0 作为 ADC 触发源
    timer_event_software_generate(TIMER0, TIMER_EVENT_SRC_CMT);
    // 或使用定时器更新事件触发
}

// ADC DMA 配置 (如果使用 DMA)
void ADC_DMA_Init(void)
{
    // DMA 通道用于传输 ADC 数据
    dma_parameter_struct dma_initpara;
    dma_deinit(DMA0, DMA_CH0);

    dma_initpara.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
    dma_initpara.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_initpara.memory_addr = (uint32_t)adc_dma_buffer;
    dma_initpara.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_initpara.number = 8;  // 8通道
    dma_initpara.priority = DMA_PRIORITY_HIGH;
    dma_initpara.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_initpara.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_initpara.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init(DMA0, DMA_CH0, &dma_initpara);

    dma_channel_enable(DMA0, DMA_CH0);
}
```

### 3. SVPWM 实现

**新建文件**: `Src/foc/svpwm.c`

```c
#include "svpwm.h"
#include "math.h"

#define SQRT3 1.7320508f

/**
 * SVPWM 扇区计算
 * 输入: Vα, Vβ (静止坐标系电压)
 * 输出: 扇区号 (1-6)
 */
uint8_t svpwm_sector(float Valpha, float Vbeta)
{
    uint8_t sector;
    float a, b, c;

    a = Vbeta;
    b = SQRT3 * Valpha - Vbeta;
    c = -SQRT3 * Valpha - Vbeta;

    if (a > 0)  sector = 1;
    if (b > 0)  sector += 2;
    if (c > 0)  sector += 4;

    return sector;
}

/**
 * 计算基本电压矢量作用时间
 * 返回: T1, T2 (单位: PWM周期分数)
 */
void svpwm_calc_times(float Valpha, float Vbeta, float Vbus,
                      uint16_t pwm_period, uint16_t *T1, uint16_t *T2)
{
    float a, b, c;
    uint8_t sector = svpwm_sector(Valpha, Vbeta);

    // Clark 逆变换到 60 度坐标系
    switch (sector) {
        case 1:
            a = Vbeta;
            b = SQRT3 * Valpha - Vbeta;
            c = -SQRT3 * Valpha - Vbeta;
            break;
        case 2:
            a = SQRT3 * Valpha + Vbeta;
            b = -SQRT3 * Valpha + Vbeta;
            c = -Vbeta;
            break;
        // ... 其他扇区
    }

    // 计算占空比
    *T1 = (uint16_t)((a / Vbus + 0.5f) * pwm_period);
    *T2 = (uint16_t)((b / Vbus + 0.5f) * pwm_period);
}

/**
 * 设置三相 PWM 占空比
 */
void svpwm_set_pwm(uint16_t Ta, uint16_t Tb, uint16_t Tc, uint16_t period)
{
    // 限制在 0-period 范围内
    Ta = (Ta > period) ? period : Ta;
    Tb = (Tb > period) ? period : Tb;
    Tc = (Tc > period) ? period : Tc;

    // 设置 TIMER0 比较寄存器
    setPWMCompare1(Ta);  // PA8
    setPWMCompare2(Tb);  // PA9
    setPWMCompare3(Tc);  // PA10
}
```

### 4. Park/Clark 变换

**新建文件**: `Src/foc/park.c`

```c
#include "park.h"
#include "math.h"

/**
 * Clark 变换: abc -> αβ
 * 输入: Ia, Ib, Ic (三相电流)
 * 输出: Ialpha, Ibeta (静止坐标系)
 */
void clarke_transform(float Ia, float Ib, float Ic, float *Ialpha, float *Ibeta)
{
    *Ialpha = Ia;
    *Ibeta = (Ia + 2.0f * Ib) / sqrtf(3.0f);
}

/**
 * Park 变换: αβ -> dq
 * 输入: Ialpha, Ibeta, theta (电角度)
 * 输出: Id, Iq (旋转坐标系)
 */
void park_transform(float Ialpha, float Ibeta, float theta, float *Id, float *Iq)
{
    float cos_theta = cosf(theta);
    float sin_theta = sinf(theta);

    *Id = Ialpha * cos_theta + Ibeta * sin_theta;
    *Iq = -Ialpha * sin_theta + Ibeta * cos_theta;
}

/**
 * 逆 Park 变换: dq -> αβ
 * 输入: Vd, Vq, theta
 * 输出: Valpha, Vbeta
 */
void inv_park_transform(float Vd, float Vq, float theta, float *Valpha, float *Vbeta)
{
    float cos_theta = cosf(theta);
    float sin_theta = sinf(theta);

    *Valpha = Vd * cos_theta - Vq * sin_theta;
    *Vbeta = Vd * sin_theta + Vq * cos_theta;
}
```

### 5. PID 控制器

**新建文件**: `Src/foc/pid.c`

```c
#include "pid.h"

void pid_init(PID_Controller *pid, float kp, float ki, float kd, float out_max)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->out_max = out_max;
    pid->integral = 0;
    pid->prev_error = 0;
}

float pid_calc(PID_Controller *pid, float setpoint, float measured, float dt)
{
    float error = setpoint - measured;

    // 比例项
    float p_out = pid->kp * error;

    // 积分项 (带限幅)
    pid->integral += pid->ki * error * dt;
    if (pid->integral > pid->out_max) pid->integral = pid->out_max;
    if (pid->integral < -pid->out_max) pid->integral = -pid->out_max;

    // 微分项
    float d_out = pid->kd * (error - pid->prev_error) / dt;

    // 总输出
    float output = p_out + pid->integral + d_out;

    // 输出限幅
    if (output > pid->out_max) output = pid->out_max;
    if (output < -pid->output_max) output = -pid->out_max;

    pid->prev_error = error;

    return output;
}
```

### 6. FOC 主控制循环

**新建文件**: `Src/foc/foc.c`

```c
#include "foc.h"
#include "ADC.h"
#include "svpwm.h"
#include "park.h"
#include "pid.h"

extern volatile uint16_t adc_dma_buffer[8];

// FOC 参数
#define POLE_PAIRS 7          // 电机极对数
#define PWM_FREQ_HZ 16000     // PWM 频率
#define PWM_PERIOD (96000000 / PWM_FREQ_HZ / 2)  // 96MHz / PWM频率 / 2

// PID 控制器
static PID_Controller pid_d = { .out_max = 12.0f };  // D轴电流环
static PID_Controller pid_q = { .out_max = 12.0f };  // Q轴电流环
static PID_Controller pid_speed = { .out_max = 20.0f }; // 速度环

// 全局变量
static float electrical_angle = 0;
static float motor_speed = 0;
static float target_speed = 0;

void FOC_Init(void)
{
    // 初始化 PID 参数
    pid_init(&pid_d, 0.1f, 0.01f, 0.0f, 12.0f);
    pid_init(&pid_q, 0.1f, 0.01f, 0.0f, 12.0f);
    pid_init(&pid_speed, 0.5f, 0.05f, 0.0f, 20.0f);
}

void FOC_ControlLoop(void)
{
    // 1. 读取 ADC 采样值
    float Ia = (float)adc_dma_buffer[0] / 4096.0f * 3.3f;  // PA1
    float Ib = (float)adc_dma_buffer[1] / 4096.0f * 3.3f;  // PA2
    // Ic = -(Ia + Ib) 由基尔霍夫定律

    float Vbus = (float)adc_dma_buffer[6] / 4096.0f * 3.3f * 11.0f;  // PA7 分压

    // 2. Clark 变换
    float Ialpha, Ibeta;
    clarke_transform(Ia, Ib, -Ia-Ib, &Ialpha, &Ibeta);

    // 3. Park 变换 (需要电角度)
    float Id, Iq;
    park_transform(Ialpha, Ibeta, electrical_angle, &Id, &Iq);

    // 4. 速度环输出作为 Q轴电流给定
    float target_Iq = pid_calc(&pid_speed, target_speed, motor_speed, 0.000125f);

    // 5. 电流环
    float Vd = pid_calc(&pid_d, 0.0f, Id, 0.000125f);  // D轴给定为0 (弱磁)
    float Vq = pid_calc(&pid_q, target_Iq, Iq, 0.000125f);

    // 6. 逆 Park 变换
    float Valpha, Vbeta;
    inv_park_transform(Vd, Vq, electrical_angle, &Valpha, &Vbeta);

    // 7. SVPWM 计算并输出
    uint16_t T1, T2;
    svpwm_calc_times(Valpha, Vbeta, Vbus, PWM_PERIOD, &T1, &T2);
    // ... 计算三相占空比并输出

    // 8. 更新电角度 (开环或观测器)
    electrical_angle += motor_speed * 0.000125f * 2 * M_PI * POLE_PAIRS / 60.0f;
    if (electrical_angle > 2 * M_PI) electrical_angle -= 2 * M_PI;
}

void FOC_SetSpeed(float rpm)
{
    target_speed = rpm;
}
```

### 7. 在主循环中调用 FOC

**修改文件**: `Src/main.c`

```c
#include "foc.h"

extern void FOC_Init(void);
extern void FOC_ControlLoop(void);
extern void FOC_SetSpeed(float rpm);

int main(void)
{
    // 系统初始化 ...
    initCorePeripherals();

    // FOC 初始化
    FOC_Init();

    // 设置目标转速 (RPM)
    FOC_SetSpeed(3000.0f);

    while (1) {
        // FOC 控制循环 (10kHz)
        FOC_ControlLoop();
    }
}
```

### 8. 定时器配置 (如果需要更高精度)

**修改文件**: `Mcu/f103/Src/peripherals.c`

```c
// 配置 TIMER6 为 FOC 控制周期 (10kHz)
void TIMER6_FOC_Init(void)
{
    rcu_periph_clock_enable(RCU_TIMER6);

    TIMER_PSC(TIMER6) = 71;    // 1MHz
    TIMER_CAR(TIMER6) = 100;   // 10kHz 中断
    NVIC_SetPriority(TIMER6_IRQn, 2);
    NVIC_EnableIRQ(TIMER6_IRQn);
    TIMER_DMAINTEN(TIMER6) |= TIMER_INT_UP;
    timer_enable(TIMER6);
}

// TIMER6 中断中添加 FOC 控制
void TIMER6_IRQHandler(void)
{
    if (TIMER_INTF(TIMER6) == SET) {
        TIMER_INTF(TIMER6) = RESET;

        FOC_ControlLoop();  // 调用 FOC 控制
    }
}
```

---

## 关键修改汇总

| 步骤 | 文件 | 修改内容 |
|------|------|----------|
| 1 | 新建 `Src/foc/*.c` | FOC 算法实现 |
| 2 | `Src/main.c` | 添加 FOC_Init() 和 FOC_ControlLoop() 调用 |
| 3 | `Mcu/f103/Src/peripherals.c` | 可选: 添加 TIMER6 FOC 中断 |
| 4 | `Mcu/f103/Src/ADC.c` | 可选: 添加 DMA 配置 |
| 5 | `Inc/targets.h` | 可选: 添加 FOC 相关宏定义 |

---

## 外部运放电路建议

三电阻电流采样需要运放进行信号调理：

```
相电流 → 采样电阻 → 运放差分放大 → PA1/PA2/PA3 → ADC
```

推荐电路参数：
- 采样电阻: 0.01Ω - 0.1Ω (根据峰值电流选择)
- 运放: 低偏置电压轨到轨运放 (如 OPA299, LM2904)
- 放大倍数: 根据采样电阻和ADC参考电压计算

---

## 参考资料

- GD32F103 用户手册
- AM32 原始 FOC 实现参考 (其他MCU平台)
- ST AN4015 - FOC 电机控制入门
