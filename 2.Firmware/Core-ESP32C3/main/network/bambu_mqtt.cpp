#include "esp_log.h"
#include "esp_netif_types.h"
#include "esp_event.h"
// #include "mqtt_client.h"

#include "bambu_mqtt.hpp"

static const char *TAG = "[MQTT]";

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    bambu_mqtt_context_t *ctx = (bambu_mqtt_context_t *)handler_args;
    ctx->mqtt_status_ = BAMBU_MQTT_STATUS_CONNECTED;
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

            // Subscribe to the report topic
            // topic: device/serial/report
            char topic[128];
            snprintf(topic, sizeof(topic), "%s/%s/%s", BAMBU_MQTT_TOPIC_BASE, ctx->serial_, BAMBU_MQTT_TOPIC_REPORT);
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
            ctx->mqtt_status_ = BAMBU_MQTT_STATUS_DISCONNECTED;
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            // printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            // printf("DATA=%.*s\r\n", event->data_len, event->data);
            ESP_LOGI(TAG, "Received data on topic: %.*s", event->topic_len, event->topic);
            if (event->data_len > 0) {
                ESP_LOGI(TAG, "Data: %.*s", event->data_len, event->data);

            } else {
                ESP_LOGI(TAG, "No data received");
            }
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            ctx->mqtt_status_ = BAMBU_MQTT_STATUS_ERROR;
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}

void wifi_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "WiFi Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    if (base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "WiFi connected, start MQTT client...");
        bambu_mqtt_context_t *ctx = (bambu_mqtt_context_t *)handler_args;
        esp_mqtt_client_start(ctx->client_);
    }
}

void BMQTT_init(bambu_mqtt_context_t *ctx, const char *ip, const char *password, const char *serial)
{
    ctx->client_ = nullptr;
    ctx->mqtt_status_ = BAMBU_MQTT_STATUS_DISCONNECTED;
    strncpy(ctx->ip_, ip, sizeof(ctx->ip_) - 1);
    ctx->ip_[sizeof(ctx->ip_) - 1] = '\0';
    strncpy(ctx->password_, password, sizeof(ctx->password_) - 1);
    ctx->password_[sizeof(ctx->password_) - 1] = '\0';
    strncpy(ctx->serial_, serial, sizeof(ctx->serial_) - 1);
    ctx->serial_[sizeof(ctx->serial_) - 1] = '\0';

    char broker_uri[128];
    snprintf(broker_uri, sizeof(broker_uri), "mqtts://%s:%d", ctx->ip_, BAMBU_MQTT_DEFAULT_PORT);

    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.broker.address.uri = broker_uri;
    mqtt_cfg.broker.verification.skip_cert_common_name_check = true;
    mqtt_cfg.broker.verification.certificate = nullptr; // 不验证证书
    // 证书问题似乎需要修改 sdkconfig 才能彻底解决，或内置证书验证
    // https://docs.espressif.com/projects/esp-idf/zh_CN/v5.5.4/esp32c3/api-reference/protocols/esp_tls.html#esp-tls-server-verification
    mqtt_cfg.credentials.username = BAMBU_MQTT_DEFAULT_USER;
    mqtt_cfg.credentials.authentication.password = ctx->password_;
    mqtt_cfg.session.keepalive = 120;

    // 关键优化配置
    // Thanks to original Top-AMS Project
    mqtt_cfg.buffer.size = 4096;                  // 增大接收缓冲区
    mqtt_cfg.buffer.out_size = 2048;              // 发送缓冲区
    mqtt_cfg.network.reconnect_timeout_ms = 5000; // 5秒重连
    mqtt_cfg.task.stack_size = 6144;              // 增大任务栈
    mqtt_cfg.task.priority = 5;                   // 提高任务优先级

    // WARNING: 直接 Log 数据可能导致关键隐私数据泄漏
    ESP_LOGI(TAG, "Connecting to MQTT broker at %s, pwd %s", broker_uri, ctx->password_);

    ctx->client_ = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(ctx->client_, (esp_mqtt_event_id_t)ESP_EVENT_ANY_ID, mqtt_event_handler, ctx);
}

void BMQTT_start(bambu_mqtt_context_t *ctx)
{
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, ctx);
}

void BMQTT_stop(bambu_mqtt_context_t *ctx)
{
    if (ctx->client_) {
        esp_mqtt_client_stop(ctx->client_);
        esp_mqtt_client_destroy(ctx->client_);
        ctx->client_ = nullptr;
        ESP_LOGI(TAG, "BambuMQTT client stopped");
    }
}

int BMQTT_publish_message(bambu_mqtt_context_t *ctx, const char *message)
{
    if (!ctx->client_) {
        ESP_LOGE(TAG, "MQTT client not initialized");
        return -1;
    }
    char topic[128];
    snprintf(topic, sizeof(topic), "%s/%s/%s", BAMBU_MQTT_TOPIC_BASE, ctx->serial_, BAMBU_MQTT_TOPIC_REQUEST);
    ESP_LOGI(TAG, "Publishing message to topic: %s", topic);
    ESP_LOGI(TAG, "Message: %s", message);
    int msg_id = esp_mqtt_client_publish(ctx->client_, topic, message, 0, 1, 0);

    if (msg_id < 0) {
        ESP_LOGE(TAG, "Failed to publish message: %s", message);
        return -1;
    }

    ESP_LOGI(TAG, "Message published successfully: %s", message);
    return msg_id;
}