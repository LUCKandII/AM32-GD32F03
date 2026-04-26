/**
 * serial_telemetry.c
 *
 * Serial telemetry for GD32F103 using UART
 */

#include "main.h"
#include "serial_telemetry.h"
#include "targets.h"

#ifdef USE_SERIAL_TELEMETRY

/* Telemetry UART configuration */
#define TELEMETRY_UART USART0
#define TELEMETRY_UART_BAUD 115200
#define TELEMETRY_UART_TX_PIN GPIO_PIN_9
#define TELEMETRY_UART_TX_PORT GPIOA

static uint8_t telem_tx_buffer[32];

void telem_UART_Init(void)
{
    /* Enable clocks */
    rcu_periph_clock_enable(RCU_USART0);
    rcu_periph_clock_enable(RCU_GPIOA);

    /* Configure TX pin (PA9) as alternate function */
    gpio_init(TELEMETRY_UART_TX_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_10MHZ,
              TELEMETRY_UART_TX_PIN);
    gpio_af_set(GPIOA, GPIO_AF_1, TELEMETRY_UART_TX_PIN);

    /* Configure USART */
    usart_deinit(TELEMETRY_UART);
    usart_baud_rate_set(TELEMETRY_UART, TELEMETRY_UART_BAUD);
    usart_word_length_set(TELEMETRY_UART, USART_WL_8BIT);
    usart_stop_bit_set(TELEMETRY_UART, USART_STB_1BIT);
    usart_parity_config(TELEMETRY_UART, USART_PM_NONE);
    usart_hardware_flow_rts_config(TELEMETRY_UART, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(TELEMETRY_UART, USART_CTS_DISABLE);
    usart_receive_config(TELEMETRY_UART, USART_RECEIVE_DISABLE);
    usart_transmit_config(TELEMETRY_UART, USART_TRANSMIT_ENABLE);
    usart_enable(TELEMETRY_UART);
}

void telem_UART_send(uint8_t data)
{
    /* Wait until transmit data register is empty */
    while (RESET == usart_flag_get(TELEMETRY_UART, USART_FLAG_TBE)) {
    }
    usart_data_transmit(TELEMETRY_UART, data);
}

void telem_send_buffer(uint8_t* data, uint8_t length)
{
    for (uint8_t i = 0; i < length; i++) {
        telem_UART_send(data[i]);
    }
}

#endif /* USE_SERIAL_TELEMETRY */

void send_telem_DMA(uint8_t bytes)
{
    /* Stub for compatibility - actual implementation would use DMA */
#ifdef USE_SERIAL_TELEMETRY
    telem_UART_send(bytes);
#else
    (void)bytes;
#endif
}
