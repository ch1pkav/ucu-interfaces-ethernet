#include "gps.h"

#include "main.h"
#include "stm32f4xx_hal_gpio.h"
#include "usart.h"
// Include order
#include "stm32f4xx_hal_uart.h"

#include <string.h>

#include "nmea.h"

#define UART_RX_BUF_SIZE RX_BUF_SIZE
static uint8_t s_uart1_rx_buf[UART_RX_BUF_SIZE] = { 0 };

void gps_init(void) {
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, s_uart1_rx_buf, sizeof(s_uart1_rx_buf));
    const char* req = "$CCGNQ,GGA\r\n";
    HAL_UART_Transmit(&huart1, (const uint8_t*)req, strlen(req), UINT32_MAX);
}

void gps_process(void) {
    nmea_process();
}

const nmea_sentence_t* gps_get_last_sentence(void) {
    return nmea_get_last_sentence();
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t Size) {
    UNUSED(huart);
    HAL_GPIO_TogglePin(LEDB_GPIO_Port, LEDB_Pin);
    nmea_rx_callback(s_uart1_rx_buf, Size);
}

float gps_lat_pos_to_float(const nmea_pos_t *lat) {
    float decimal_minutes = (float)lat->min / 60.0f;

    float magnitude = (float)lat->deg + decimal_minutes;

    if (lat->dir == NMEA_CARDINAL_DIR_SOUTH) {
        return -magnitude;
    } else {
        return magnitude;
    }
}

float gps_lon_pos_to_float(const nmea_pos_t *lon) {
    float decimal_minutes = (float)lon->min / 60.0f;

    float magnitude = (float)lon->deg + decimal_minutes;

    if (lon->dir == NMEA_CARDINAL_DIR_WEST) {
        return -magnitude;
    } else {
        return magnitude;
    }
}
