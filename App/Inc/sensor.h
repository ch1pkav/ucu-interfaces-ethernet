#pragma once

void sensor_init(void);
void sensor_update(void);

float sensor_get_temperature(void);
float sensor_get_humidity(void);
float sensor_get_pressure(void);
