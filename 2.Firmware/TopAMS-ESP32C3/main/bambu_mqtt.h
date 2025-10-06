#pragma once

#include "mqtt_client.h"

#include "model/bambu_status.h"

#define BAMBU_MQTT_DEFAULT_USER "bblp"
#define BAMBU_MQTT_DEFAULT_PORT 8883

#define BAMBU_MQTT_TOPIC_BASE "device"
#define BAMBU_MQTT_TOPIC_REPORT "report"
#define BAMBU_MQTT_TOPIC_REQUEST "request"

enum BambuMQTTStatus {
    BAMBU_MQTT_STATUS_DISCONNECTED = 0,
    BAMBU_MQTT_STATUS_CONNECTED,
    BAMBU_MQTT_STATUS_ERROR
};

class BambuMQTT {
public:
    using InfoCallback = void (*)(const char *topic, const char *payload);

    BambuMQTT(const char *ip, const char *password, const char *serial, const BambuStatus &status,
              InfoCallback cb);
    ~BambuMQTT();

    void start();
    void stop();

    int publish_message(const char *message);

    esp_mqtt_client_handle_t getClient() const { return client_; }
    const char *getIP() const { return ip_; }
    const char *getSerial() const { return serial_; }
    const char *getPassword() const { return password_; }

    bool isConnected() const { return client_ != nullptr; }

private:
    esp_mqtt_client_handle_t client_;
    const char *ip_;
    const char *serial_;
    const char *password_;
    InfoCallback info_cb_;

    BambuStatus status_;

    BambuMQTTStatus mqtt_status_ = BAMBU_MQTT_STATUS_DISCONNECTED;

    static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id,
                                   void *event_data);
};
