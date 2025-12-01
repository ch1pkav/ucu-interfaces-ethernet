#include "server.h"

#include "sensor.h"
#include "spi.h"
#include "stm32f4xx_hal.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "dhcp.h"
#include "httpServer.h"
#include "socket.h"

#define DHCP_SOCKET 0
#define DNS_SOCKET 1
#define HTTP_SOCKET 2
#define SOCK_TCPS 0
#define SOCK_UDPS 1
#define PORT_TCPS 5000
#define PORT_UDPS 3000
#define MAX_HTTPSOCK 6
const char* web_content_fstr =
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<title>W5500-STM32 Web Server</title>"
    "<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>"
    "<link href=\"data:image/x-icon;base64,A\" rel=\"icon\" type=\"image/x-icon\">"
    "<style>"
    "html {display: inline-block; margin: 0px auto; text-align: center;}"
    "body{margin-top: 50px;}"
    "p {font-size: 20px;color: #808080;margin-bottom: 20px;}"
    "</style>"
    "</head>"
    "<body>"
    "<h1>STM32 - W5500</h1>"
    "<p>Chip ID: %d-%d-%d</p>"
    "<p>Temperature: %d</p>"
    "<p>Humidity: %d</p>"
    "<p>Pressure: %d</p>"
    "</body>"
    "</html>";

static char web_content_buf[2048];

static uint8_t socknumlist[] = { 2, 3, 4, 5, 6, 7 };
static uint8_t RX_BUF[1024];
static uint8_t TX_BUF[1024];
wiz_NetInfo net_info = { .mac = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }, .dhcp = NETINFO_DHCP };

void wizchipSelect(void) {
    HAL_GPIO_WritePin(ETH_CS_GPIO_Port, ETH_CS_Pin, GPIO_PIN_RESET);
}

void wizchipUnselect(void) {
    HAL_GPIO_WritePin(ETH_CS_GPIO_Port, ETH_CS_Pin, GPIO_PIN_SET);
}

void wizchipReadBurst(uint8_t* buff, uint16_t len) {
    HAL_SPI_Receive(&hspi1, buff, len, HAL_MAX_DELAY);
}

void wizchipWriteBurst(uint8_t* buff, uint16_t len) {
    HAL_SPI_Transmit(&hspi1, buff, len, HAL_MAX_DELAY);
}

uint8_t wizchipReadByte(void) {
    uint8_t byte;
    wizchipReadBurst(&byte, sizeof(byte));
    return byte;
}

void wizchipWriteByte(uint8_t byte) {
    wizchipWriteBurst(&byte, sizeof(byte));
}

volatile bool ip_assigned = false;

void Callback_IPAssigned(void) {
    ip_assigned = true;
}

void Callback_IPConflict(void) {
    ip_assigned = false;
}

uint8_t dhcp_buffer[1024];
uint8_t dns_buffer[1024];

void W5500Init(void) {
    // Register W5500 callbacks
    reg_wizchip_cs_cbfunc(wizchipSelect, wizchipUnselect);
    reg_wizchip_spi_cbfunc(wizchipReadByte, wizchipWriteByte);
    reg_wizchip_spiburst_cbfunc(wizchipReadBurst, wizchipWriteBurst);

    uint8_t rx_tx_buff_sizes[] = { 2, 2, 2, 2, 2, 2, 2, 2 };
    wizchip_init(rx_tx_buff_sizes, rx_tx_buff_sizes);

    // set MAC address before using DHCP
    setSHAR(net_info.mac);
    DHCP_init(DHCP_SOCKET, dhcp_buffer);

    reg_dhcp_cbfunc(Callback_IPAssigned, Callback_IPAssigned, Callback_IPConflict);

    uint32_t ctr = 10000;
    while ((!ip_assigned) && (ctr > 0)) {
        DHCP_run();
        ctr--;
    }
    if (!ip_assigned) {
        return;
    }

    getIPfromDHCP(net_info.ip);
    getGWfromDHCP(net_info.gw);
    getSNfromDHCP(net_info.sn);

    wizchip_setnetinfo(&net_info);
}

static void fill_web_content_buf(float temparature, float humidity, float pressure) {
    sprintf(web_content_buf,
            web_content_fstr,
            HAL_GetUIDw0(),
            HAL_GetUIDw1(),
            HAL_GetUIDw2(),
            (int) temparature,
            (int) humidity,
            (int) pressure);
}

// Interface

void server_init(void) {
    W5500Init();
    httpServer_init(TX_BUF, RX_BUF, MAX_HTTPSOCK, socknumlist);
    reg_httpServer_cbfunc(NVIC_SystemReset, NULL);
    fill_web_content_buf(0.f, 0.f, 0.f);
    /* Web content registration */
    reg_httpServer_webContent((uint8_t*)"index.html", (uint8_t*)web_content_buf);
}

void server_run(void) {
    // update webcontent
    fill_web_content_buf(sensor_get_temperature(), sensor_get_humidity(), sensor_get_pressure());
    reg_httpServer_webContent((uint8_t*)"index.html", (uint8_t*)web_content_buf);

    // serve
    for (int i = 0; i < MAX_HTTPSOCK; i++)
        httpServer_run(i);
}
