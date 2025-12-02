#pragma once

#include "nmea.h"

void gps_init(void);

void gps_process(void);

const nmea_sentence_t* gps_get_last_sentence(void);

float gps_lon_pos_to_float(const nmea_pos_t *lon);

float gps_lat_pos_to_float(const nmea_pos_t *lat);
