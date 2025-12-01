#include "display.h"

#include <stdint.h>
#include <stdio.h>

#include "lcd.h"
#include "lcd_driver.h"
#include "lcd_fonts.h"
#include "sensor.h"

typedef struct
{
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
    // static char id_string[50];
    // static char gps_string[50];

    sprintf(temp_string, "Temperature: %03.1f C", sensor_get_temperature());
    sprintf(hum_string, "Humidity: %03.1f %%", sensor_get_humidity());
    sprintf(press_string, "Pressure: %03.1f hPa", sensor_get_pressure());
    sprintf(ip_string, "IP Address: %d.%d.%d.%d", 
        (uint8_t) (ip_address >> 24) & 0xFF,
        (uint8_t) (ip_address >> 16) & 0xFF,
        (uint8_t) (ip_address >> 8) & 0xFF,
        (uint8_t) ip_address & 0xFF
    );


    ILI9341_DrawText("BME280:", FONT2, 5, 5, BLUE, WHITE);

    ILI9341_DrawText(temp_string, FONT1, 5, 20, BLACK, WHITE);
    ILI9341_DrawText(hum_string, FONT1, 5, 35, BLACK, WHITE);
    ILI9341_DrawText(press_string, FONT1, 5, 50, BLACK, WHITE);

    ILI9341_DrawText("Server:", FONT2, 5, 65, BLUE, WHITE);
    ILI9341_DrawText(ip_string, FONT1, 5, 80, BLACK, WHITE);
}
