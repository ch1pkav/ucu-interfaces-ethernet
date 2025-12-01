#include "app_main.h"

#include "sensor.h"
#include "server.h"

void app_main(void) {
  // main

  server_init();
  sensor_init();

  while (1) {
    sensor_update();
    server_run();
  }

  return;
}
