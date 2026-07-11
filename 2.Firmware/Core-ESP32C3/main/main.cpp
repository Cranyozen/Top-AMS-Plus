#include <stdio.h>
#include "esp_log.h"
#include "esp_err.h"

#include "config.hpp"
#include "network/wifi.hpp"
#include "nvs/nvs.hpp"
#include "network/smartconfig.hpp"
#include "network/bambu_mqtt.hpp"

static const char *TAG = "main";
static BambuMQTT_context_t mqtt_context;

extern "C" void app_main(void)
{
    nvs_context_t nvs_context;
    esp_err_t err = NVS_init(&nvs_context, NVS_NAMESPACE);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize NVS: %s", esp_err_to_name(err));
        return;
    }

    BMQTT_init(&mqtt_context, "192.168.1.199", "56154859", "03919D530105226");

    WiFi_init();
    SC_init();

    WiFi_set_mode(WIFI_MODE_STA);

    WiFi_start();
    BMQTT_start(&mqtt_context);
}