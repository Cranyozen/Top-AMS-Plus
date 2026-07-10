#include <stdio.h>
#include "esp_log.h"
#include "esp_err.h"
#include "network/wifi.hpp"
#include "nvs/nvs.hpp"
#include "network/smartconfig.hpp"

static const char *TAG = "main";

extern "C" void app_main(void)
{
    nvs_context_t nvs_context;
    nvs_context.nvs_namespace = "storage";
    esp_err_t err = Nvs_Init(&nvs_context);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize NVS: %s", esp_err_to_name(err));
        return;
    }

    Wifi_Init();
    SC_Init();

    Wifi_SetMode(WIFI_MODE_STA);

    Wifi_Start();
}