#include "esp_event.h"
#include "esp_netif.h"
#include <esp_log.h>
#include <nvs_flash.h>

#include "driver/gpio.h"

// #include "bambu_mqtt.h"
// #include "wifi_manager.h"
#include "instance.h"

static const char *TAG = "[Main]";

extern "C" void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // gpio_dump_io_configuration(stdout, (1ULL << 5) | (1ULL << 6) | (1ULL << 7));
    // gpio_dump_io_configuration(stdout, SOC_GPIO_VALID_GPIO_MASK);

    Instance::get().init();
    // wifi_manager.init();

    // Print MAC Address
    ESP_LOGI(TAG, "Device MAC Address: %02X:%02X:%02X:%02X:%02X:%02X",
             Instance::get().mac_address[0], Instance::get().mac_address[1],
             Instance::get().mac_address[2], Instance::get().mac_address[3],
             Instance::get().mac_address[4], Instance::get().mac_address[5]);
    ESP_LOGI(TAG, "Device Name: %s", Instance::get().device_name.c_str());

    // WiFi Connect Event
    esp_event_handler_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP,
        [](void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
            ESP_LOGI(TAG, "WiFi connected, IP event received");
            Instance::get().bambu_mqtt->start(); // 启动 MQTT 客户端
            Instance::get().ws_server->start();  // 启动 WebSocket 服务器
            Instance::get().mdns_service->init();
            Instance::get().mdns_service->addService();
        },
        nullptr);

    // 其他主程序逻辑可在此补充
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        // if (Instance::get().wifi_manager->is_connected()) {
        //     // ESP_LOGI(TAG, "WiFi is connected");
        //     // 这里可以添加更多逻辑，比如发布 MQTT 消息
        //     if (!Instance::get().bambu_mqtt->isConnected()) {
        //         vTaskDelay(10000 / portTICK_PERIOD_MS); // 等待 10 秒钟
        //         Instance::get().bambu_mqtt->start(); // 确保 MQTT 客户端已启动
        //     } else {
        //         // ESP_LOGI(TAG, "MQTT is connected");
        //     }
        // } else {
        //     ESP_LOGI(TAG, "WiFi is not connected");
        // }
        // 这里可以添加更多逻辑，比如处理 MQTT 消息等
    }
}
