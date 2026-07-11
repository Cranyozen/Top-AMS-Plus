#pragma once

#include "mqtt_client.h"

#include "model/bambu_status.h"

#define BAMBU_MQTT_DEFAULT_USER "bblp"
#define BAMBU_MQTT_DEFAULT_PORT 8883

#define BAMBU_MQTT_TOPIC_BASE    "device"
#define BAMBU_MQTT_TOPIC_REPORT  "report"
#define BAMBU_MQTT_TOPIC_REQUEST "request"

enum bambu_mqtt_status_t
{
    BAMBU_MQTT_STATUS_DISCONNECTED = 0,
    BAMBU_MQTT_STATUS_CONNECTED,
    BAMBU_MQTT_STATUS_ERROR
};

struct bambu_mqtt_context_t
{
    char ip_[16]; // max xxx.xxx.xxx.xxx\0
    char password_[32];
    char serial_[32];
    esp_mqtt_client_handle_t client_;
    bambu_mqtt_status_t mqtt_status_;
    bambu_status_t bambu_status_;
};

void BMQTT_init(bambu_mqtt_context_t *ctx, const char *ip, const char *password, const char *serial);
void BMQTT_start(bambu_mqtt_context_t *ctx);
void BMQTT_stop(bambu_mqtt_context_t *ctx);
int BMQTT_publish_message(bambu_mqtt_context_t *ctx, const char *message);
