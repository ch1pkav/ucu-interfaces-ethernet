#include "sensor.h"

#include "stdlib.h"
#include "string.h"

#include "bme280.h"
#include "i2c.h"

static float temperature;
static float humidity;
static float pressure;

static struct bme280_dev dev;
static struct bme280_data comp_data;
static int8_t rslt;

static int8_t user_i2c_read(uint8_t id, uint8_t reg_addr, uint8_t* data, uint16_t len) {
    if (HAL_I2C_Master_Transmit(&hi2c1, (id << 1), &reg_addr, 1, 10) != HAL_OK)
        return -1;
    if (HAL_I2C_Master_Receive(&hi2c1, (id << 1) | 0x01, data, len, 10) != HAL_OK)
        return -1;

    return 0;
}

static void user_delay_ms(uint32_t period) {
    HAL_Delay(period);
}

static int8_t user_i2c_write(uint8_t id, uint8_t reg_addr, uint8_t* data, uint16_t len) {
    int8_t* buf;
    buf = malloc(len + 1);
    buf[0] = reg_addr;
    memcpy(buf + 1, data, len);

    if (HAL_I2C_Master_Transmit(&hi2c1, (id << 1), (uint8_t*)buf, len + 1, HAL_MAX_DELAY) != HAL_OK)
        return -1;

    free(buf);
    return 0;
}

void sensor_init(void) {
    dev.dev_id = BME280_I2C_ADDR_PRIM;
    dev.intf = BME280_I2C_INTF;
    dev.read = user_i2c_read;
    dev.write = user_i2c_write;
    dev.delay_ms = user_delay_ms;

    rslt = bme280_init(&dev);

    dev.settings.osr_h = BME280_OVERSAMPLING_1X;
    dev.settings.osr_p = BME280_OVERSAMPLING_16X;
    dev.settings.osr_t = BME280_OVERSAMPLING_2X;
    dev.settings.filter = BME280_FILTER_COEFF_16;
    rslt = bme280_set_sensor_settings(
        BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL, &dev);
}

void sensor_update(void) {
    rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, &dev);
    dev.delay_ms(40);
    rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, &dev);

    if (rslt == BME280_OK)
    {
      temperature = comp_data.temperature / 100.0;
      humidity = comp_data.humidity / 1024.0;
      pressure = comp_data.pressure / 10000.0;

    //   sprintf(temp_string, "Temperature: %03.1f C", temperature);
    //   sprintf(hum_string, "Humidity: %03.1f %%", humidity);
    //   sprintf(press_string, "Pressure: %03.1f hPa", pressure);

    //   ILI9341_DrawText("BME280:", FONT2, 5, 5, BLUE, WHITE);

    //   ILI9341_DrawText(temp_string, FONT1, 5, 20, BLACK, WHITE);
    //   ILI9341_DrawText(hum_string, FONT1, 5, 35, BLACK, WHITE);
    //   ILI9341_DrawText(press_string, FONT1, 5, 50, BLACK, WHITE);
    }
}

float sensor_get_temperature(void) {
    return temperature;
}

float sensor_get_humidity(void) {
    return humidity;
}

float sensor_get_pressure(void) {
    return pressure;
}
