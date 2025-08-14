/*
#include "nvs_flash.h"
#include "esp_log.h"
#include "nvs.h"
#include <string>
#include <cstring>

#include "nvs_manager.h"

esp_err_t NVSManager::init() {
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        ESP_LOGI(TAG, "NVS flash erased, reinitializing");
        err = nvs_flash_init();
    }
    if (err == ESP_OK) {
        handle = nvs::open_nvs_handle(namespace_name, NVS_READWRITE, &err);
    }
    ESP_ERROR_CHECK(err);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS namespace '%s': %s", namespace_name, esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "NVS namespace '%s' opened successfully", namespace_name);
    }
    return err;
}

esp_err_t NVSManager::commit() {
    if (!handle) return ESP_ERR_INVALID_STATE;
    ESP_LOGI(TAG, "Committing changes to NVS namespace '%s'", namespace_name);
    return handle->commit();
}
    */