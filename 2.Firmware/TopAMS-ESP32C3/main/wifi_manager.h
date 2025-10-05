#pragma once

#include "esp_event.h"

class WifiManager {
public:
    WifiManager();
    void init();

    bool is_connected();

    bool set_ssid(const char* ssid);

    bool set_password(const char* password);

    bool reconnect();
    
private:
    static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
    static void smartconfig_task(void *parm);
    static EventGroupHandle_t s_wifi_event_group;
    static const int CONNECTED_BIT;
    static const int ESPTOUCH_DONE_BIT;
};
