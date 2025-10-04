#include "bambu_mqtt.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include <cstdio>
#include <stdint.h>
#include <stdlib.h>

#define BAMBU_MQTT_DEFAULT_USER "bblp"
#define BAMBU_MQTT_DEFAULT_PORT 8883

static const char *TAG = "[BambuMQTT]";

void BambuMQTT::mqtt_event_handler(void *handler_args, esp_event_base_t base,
                               int32_t event_id, void *event_data) {
    BambuMQTT *self = static_cast<BambuMQTT*>(handler_args);
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            self->mqtt_status_ = BAMBU_MQTT_STATUS_CONNECTED;

            // Subscribe to the report topic
            // topic: device/serial/report
            char topic[128];
            snprintf(topic, sizeof(topic), "%s/%s/%s", BAMBU_MQTT_TOPIC_BASE, self->serial_.c_str(), BAMBU_MQTT_TOPIC_REPORT);
            ESP_LOGI(TAG, "Subscribing to topic: %s", topic);
            msg_id = esp_mqtt_client_subscribe(client, topic, 1);
            if (msg_id < 0) {
                ESP_LOGE(TAG, "Failed to subscribe to topic: %s", BAMBU_MQTT_TOPIC_BASE "/" BAMBU_MQTT_TOPIC_REPORT);
            } else {
                ESP_LOGI(TAG, "Subscribed to topic successfully, msg_id=%d", msg_id);
            }
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            self->mqtt_status_ = BAMBU_MQTT_STATUS_DISCONNECTED;
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            // printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            // printf("DATA=%.*s\r\n", event->data_len, event->data);
            ESP_LOGI(TAG, "Received data on topic: %.*s", event->topic_len, event->topic);
            if (event->data_len > 0) {
                // WARNING: 直接 Log 数据可能导致数据泄漏，日志过长，刷新过快等问题
                ESP_LOGI(TAG, "Data: %.*s", event->data_len, event->data);
                if (event->topic_len > 0) {
                    std::string topic(event->topic, event->topic_len);
                    std::string payload(event->data, event->data_len);
                }

                // Try to parser data
                // 尝试解析基本信息，尽管不是必须的
                /*
                {
                    "print": {
                        "nozzle_temper": 26.78125,
                        "bed_temper": 27.65625,
                        "wifi_signal": "-29dBm",
                        "command": "push_status",
                        "msg": 1,
                        "sequence_id": "2511"
                    }
                }
                */
                cJSON *root = cJSON_Parse((const char *)event->data);
                if (root) {
                    cJSON *print = cJSON_GetObjectItem(root, "print");
                    if (print) {
                        cJSON *nozzle_temper = cJSON_GetObjectItem(print, "nozzle_temper");
                        cJSON *bed_temper = cJSON_GetObjectItem(print, "bed_temper");
                        cJSON *wifi_signal = cJSON_GetObjectItem(print, "wifi_signal");
                        cJSON *command = cJSON_GetObjectItem(print, "command");
                        cJSON *msg = cJSON_GetObjectItem(print, "msg");
                        cJSON *sequence_id = cJSON_GetObjectItem(print, "sequence_id");
                        
                        if (nozzle_temper) {
                            self->status_.nozzle_temper = nozzle_temper->valuedouble;
                        }
                        if (bed_temper) {
                            self->status_.bed_temper = bed_temper->valuedouble;
                        }
                        if (wifi_signal) {
                            self->status_.wifi_signal = wifi_signal->valuestring ? wifi_signal->valuestring : "";
                        }
                        // ESP_LOGI(TAG, "Nozzle Temperature: %.2f", self->status_.nozzle_temper);
                        // ESP_LOGI(TAG, "Bed Temperature: %.2f", self->status_.bed_temper);
                        // ESP_LOGI(TAG, "WiFi Signal: %s", self->status_.wifi_signal.c_str());
                    }
                    // char* json_data = cJSON_Print(root);
                    // if (json_data) {
                    //     ESP_LOGI(TAG, "Parsed JSON data: %s", json_data);
                    //     free(json_data);
                    // } else {
                    //     ESP_LOGE(TAG, "Failed to print JSON data");
                    // }
                    cJSON_Delete(root);
                }

            } else {
                ESP_LOGI(TAG, "No data received");
            }
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            self->mqtt_status_ = BAMBU_MQTT_STATUS_ERROR;
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}

BambuMQTT::BambuMQTT(const std::string &ip, const std::string &password, const std::string &serial, const BambuStatus &status, InfoCallback cb)
    : client_(nullptr), ip_(ip), serial_(serial), password_(password), info_cb_(cb), status_(status) {
    ESP_LOGI(TAG, "BambuMQTT constructed: ip=%s, serial=%s, password=%s", ip.c_str(), serial.c_str(), password.c_str());
}

BambuMQTT::~BambuMQTT() {
    stop();
}

void BambuMQTT::start() {
    char broker_uri[128];
    snprintf(broker_uri, sizeof(broker_uri), "mqtts://%s:%d", ip_.c_str(), BAMBU_MQTT_DEFAULT_PORT);

    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.broker.address.uri = broker_uri;
    mqtt_cfg.broker.verification.skip_cert_common_name_check = true;
    mqtt_cfg.broker.verification.certificate = nullptr; // 不验证证书
    // 证书问题似乎需要修改 sdkconfig 才能彻底解决，或内置证书验证
    mqtt_cfg.credentials.username = BAMBU_MQTT_DEFAULT_USER;
    mqtt_cfg.credentials.authentication.password = password_.c_str();
    mqtt_cfg.session.keepalive = 120;

    // 关键优化配置
    // Thanks to original Top-AMS Project
    mqtt_cfg.buffer.size = 4096;                  // 增大接收缓冲区
    mqtt_cfg.buffer.out_size = 2048;              // 发送缓冲区
    mqtt_cfg.network.reconnect_timeout_ms = 5000; // 5秒重连
    mqtt_cfg.task.stack_size = 6144;              // 增大任务栈
    mqtt_cfg.task.priority = 5;                   // 提高任务优先级
    
    // WARNING: 直接 Log 数据可能导致关键隐私数据泄漏
    ESP_LOGI(TAG, "Connecting to MQTT broker at %s, pwd %s", broker_uri, password_.c_str());

    client_ = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client_, (esp_mqtt_event_id_t)ESP_EVENT_ANY_ID, mqtt_event_handler, this);
    esp_mqtt_client_start(client_);
    ESP_LOGI(TAG, "BambuMQTT client started");
}

void BambuMQTT::stop() {
    if (client_) {
        esp_mqtt_client_stop(client_);
        esp_mqtt_client_destroy(client_);
        client_ = nullptr;
        ESP_LOGI(TAG, "BambuMQTT client stopped");
    }
}

int BambuMQTT::publish_message(const std::string &message) {
    if (!client_) {
        ESP_LOGE(TAG, "MQTT client not initialized");
        return -1;
    }

    std::string topic = std::string(BAMBU_MQTT_TOPIC_BASE) + "/" + serial_ + "/" + std::string(BAMBU_MQTT_TOPIC_REPORT);
    int msg_id = esp_mqtt_client_publish(client_, topic.c_str(), message.c_str(), 0, 1, 0);
    
    if (msg_id < 0) {
        ESP_LOGE(TAG, "Failed to publish message: %s", message.c_str());
        return -1;
    }
    
    ESP_LOGI(TAG, "Message published successfully: %s", message.c_str());
    return msg_id;
}
