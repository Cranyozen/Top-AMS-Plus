#pragma once

#include "mdns_service.h"
#include "esp_log.h"
#include <string>

class MDnsService {
public:
    MDnsService(const std::string& instance_name, const std::string& service_name, const std::string& proto, uint16_t port);
    ~MDnsService();

    void init();
    void addService();

private:
    std::string instance_name_;
    std::string service_name_;
    std::string proto_;
    uint16_t port_;
    static const char* TAG;
};
