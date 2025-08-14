#include "instance.h"


Instance::Instance() {
    // bambu_mqtt = new BambuMQTT("192.168.1.199", "56154859", "03919D530105226", bambu_status, nullptr);
    // wifi_manager = new WifiManager();
    bambu_mqtt = std::make_shared<BambuMQTT>("192.168.1.199", "56154859", "03919D530105226", bambu_status, nullptr);
    wifi_manager = std::make_shared<WifiManager>();
    ws_server = std::make_shared<WSServer>();
    nvs_manager = std::make_shared<NVSManager>();
}
void Instance::init() {
    // bambu_mqtt->start();
    nvs_manager->init();
    wifi_manager->init();
}

void Instance::deinit() {
    bambu_mqtt->stop();
    // wifi_manager->deinit();
}
