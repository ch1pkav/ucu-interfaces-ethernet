#include "app_main.h"

#include "display.h"
#include "gps.h"
#include "sensor.h"
#include "server.h"

void app_main(void) {
    // main

    server_init();
    sensor_init();
    gps_init();
    display_init();

    while (1) {
        sensor_update();
        gps_process();
        display_update(server_get_ipv4());
        server_run();
    }

    return;
}
