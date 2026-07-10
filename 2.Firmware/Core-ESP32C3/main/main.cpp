#include <stdio.h>
#include "esp_log.h"
#include "esp_err.h"
#include "network/wifi.hpp"
#include "nvs/nvs.hpp"
#include "network/smartconfig.hpp"
#include "network/bambu_mqtt.hpp"

static const char *TAG = "main";
static BambuMQTT_context_t mqtt_context;

extern "C" void app_main(void)
{
    nvs_context_t nvs_context;
    nvs_context.nvs_namespace = "storage";
    esp_err_t err = Nvs_Init(&nvs_context);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize NVS: %s", esp_err_to_name(err));
        return;
    }

    BambuMQTT_init(&mqtt_context, "192.168.1.199", "56154859", "03919D530105226");

    Wifi_Init();
    SC_Init();

    Wifi_SetMode(WIFI_MODE_STA);

    Wifi_Start();
    BambuMQTT_start(&mqtt_context);
}