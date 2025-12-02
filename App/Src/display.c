#include "display.h"

#include <stdint.h>
#include <stdio.h>

#include "forward.h"
#include "gps.h"
#include "lcd.h"
#include "lcd_driver.h"
#include "lcd_fonts.h"
#include "nmea.h"
#include "sensor.h"

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    uint8_t dirty;
} DisplayTile_t;

// Interface
void display_init(void) {
    ILI9341_Init();
    ILI9341_FillScreen(WHITE);
    ILI9341_SetRotation(SCREEN_VERTICAL_2);
}

void display_update(uint32_t ip_address) {
    static char temp_string[50];
    static char hum_string[50];
    static char press_string[50];
    static char ip_string[50];
    static char id_string[50];
    static char gps_string[50];

    sprintf(temp_string, "Temperature: %03.1f C", sensor_get_temperature());
    sprintf(hum_string, "Humidity: %03.1f %%", sensor_get_humidity());
    sprintf(press_string, "Pressure: %03.1f hPa", sensor_get_pressure());
    sprintf(ip_string,
            "IP Address: %d.%d.%d.%d",
            (uint8_t)(ip_address >> 24) & 0xFF,
            (uint8_t)(ip_address >> 16) & 0xFF,
            (uint8_t)(ip_address >> 8) & 0xFF,
            (uint8_t)ip_address & 0xFF);

    get_device_id_str(id_string);

    const nmea_sentence_t* gps_sent = gps_get_last_sentence();
    sprintf(gps_string,
            "Lat: %d.%d%c, Lon: %d.%d%c",
            gps_sent->lat.deg,
            (int)gps_sent->lat.min,
            gps_sent->lat.dir,
            gps_sent->lon.deg,
            (int)gps_sent->lon.min,
            gps_sent->lon.dir);

    ILI9341_DrawText("Device:", FONT2, 5, 5, BLUE, WHITE);
    ILI9341_DrawText(id_string, FONT1, 5, 20, BLACK, WHITE);

    ILI9341_DrawText("BME280:", FONT2, 5, 40, BLUE, WHITE);
    ILI9341_DrawText(temp_string, FONT1, 5, 55, BLACK, WHITE);
    ILI9341_DrawText(hum_string, FONT1, 5, 70, BLACK, WHITE);
    ILI9341_DrawText(press_string, FONT1, 5, 85, BLACK, WHITE);

    ILI9341_DrawText("Server:", FONT2, 5, 105, BLUE, WHITE);
    ILI9341_DrawText(ip_string, FONT1, 5, 120, BLACK, WHITE);

    ILI9341_DrawText("GPS:", FONT2, 5, 140, BLUE, WHITE);
    ILI9341_DrawText(gps_string, FONT1, 5, 155, BLACK, WHITE);
}
