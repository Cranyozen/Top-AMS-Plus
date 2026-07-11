#pragma once

#include "mqtt_client.h"

#include "model/bambu_status.h"

#define BAMBU_MQTT_DEFAULT_USER "bblp"
#define BAMBU_MQTT_DEFAULT_PORT 8883

#define BAMBU_MQTT_TOPIC_BASE    "device"
#define BAMBU_MQTT_TOPIC_REPORT  "report"
#define BAMBU_MQTT_TOPIC_REQUEST "request"

enum BambuMQTTStatus
{
    BAMBU_MQTT_STATUS_DISCONNECTED = 0,
    BAMBU_MQTT_STATUS_CONNECTED,
    BAMBU_MQTT_STATUS_ERROR
};

typedef struct
{
    char ip_[16]; // max xxx.xxx.xxx.xxx\0
    char password_[32];
    char serial_[32];
    esp_mqtt_client_handle_t client_;
    BambuMQTTStatus status_;
} BambuMQTT_context_t;

void BMQTT_init(BambuMQTT_context_t *ctx, const char *ip, const char *password, const char *serial);
void BMQTT_start(BambuMQTT_context_t *ctx);
void BMQTT_stop(BambuMQTT_context_t *ctx);
int BMQTT_publish_message(BambuMQTT_context_t *ctx, const char *message);
