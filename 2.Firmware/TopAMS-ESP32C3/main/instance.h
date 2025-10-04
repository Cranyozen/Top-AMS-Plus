#pragma once

#include <memory>
#include "bambu_mqtt.h"
#include "wifi_manager.h"
#include "nvs_manager.h"
#include "ws_server.h"
#include "filament_manager.h"
#include "mdns_service.h"

class Instance {
public:
    static Instance& get() {
        static Instance instance;
        return instance;
    }

    void init();
    void deinit();

    std::shared_ptr<BambuMQTT> bambu_mqtt;
    std::shared_ptr<WifiManager> wifi_manager;
    std::shared_ptr<WSServer> ws_server;
    std::shared_ptr<NVSManager> nvs_manager;
    std::shared_ptr<FilamentManager> filament_manager;
    std::shared_ptr<MDnsService> mdns_service;

    BambuStatus bambu_status;

    // MAC Address
    uint8_t mac_address[6];

    std::string device_name;

    // 禁止拷贝
    Instance(const Instance&) = delete;
    Instance& operator=(const Instance&) = delete;

private:
    // 私有构造函数，在这里初始化服务
    Instance();
};
