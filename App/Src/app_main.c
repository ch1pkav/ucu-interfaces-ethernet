#include "app_main.h"

#include "display.h"
#include "sensor.h"
#include "server.h"

void app_main(void) {
  // main

  server_init();
  sensor_init();
  display_init();

  while (1) {
    sensor_update();
    display_update(server_get_ipv4());
    server_run();
  }

  return;
}
