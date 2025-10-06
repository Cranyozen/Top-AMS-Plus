#include "instance.h"
#include "esp_mac.h"

Instance::Instance() {
    // bambu_mqtt = new BambuMQTT("192.168.1.199", "56154859", "03919D530105226", bambu_status, nullptr);
    // wifi_manager = new WifiManager();
    bambu_mqtt = std::make_shared<BambuMQTT>("192.168.1.199", "56154859", "03919D530105226",
                                             bambu_status, nullptr);
    wifi_manager = std::make_shared<WifiManager>();
    ws_server = std::make_shared<WSServer>();
    nvs_manager = std::make_shared<NVSManager>();
    filament_manager = std::make_shared<FilamentManager>();
    esp_efuse_mac_get_default(mac_address);

    // set device name based on MAC address
    snprintf(device_name, sizeof(device_name), "TopAMS-%02X%02X%02X", mac_address[3],
             mac_address[4], mac_address[5]);

    mdns_service = std::make_shared<MDnsService>(device_name, "TopAMS", "_http", 80);
}
void Instance::init() {
    // bambu_mqtt->start();
    nvs_manager->init();
    wifi_manager->init();
    // ws_server->start();
    filament_manager->init();
}

void Instance::deinit() {
    bambu_mqtt->stop();
    // wifi_manager->deinit();
}
