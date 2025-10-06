#pragma once

#include <string>

class MDnsService {
public:
    MDnsService(const char *instance_name, const char *service_name, const char *proto,
                uint16_t port);
    ~MDnsService();

    void init();
    void addService();

private:
    const char *instance_name_;
    const char *service_name_;
    const char *proto_;
    uint16_t port_;
    static const char *TAG;
};
