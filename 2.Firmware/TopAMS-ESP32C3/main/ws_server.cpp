/* WebSocket Echo Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_mac.h"
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <sys/param.h>

#include "filament_manager.h"
#include "instance.h"
#include "ws_server.h"
#include <esp_http_server.h>

const char *WSServer::TAG = "[WebSocketServer]";

void handle_ws_message(const char *message, std::string &response);

WSServer::WSServer() : server(nullptr) {}
WSServer::~WSServer() { stop(); }

esp_err_t WSServer::start() {
    if (!server) {
        server = start_webserver();
        return server ? ESP_OK : ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t WSServer::stop() {
    if (server) {
        esp_err_t ret = stop_webserver(server);
        if (ret == ESP_OK) {
            server = nullptr;
        }
        return ret;
    }
    return ESP_OK;
}

httpd_handle_t WSServer::getHandle() const { return server; }

void WSServer::onConnect(esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (server == nullptr) {
        ESP_LOGI(TAG, "Starting webserver");
        server = start_webserver();
    }
}

void WSServer::onDisconnect(esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (server) {
        ESP_LOGI(TAG, "Stopping webserver");
        if (stop_webserver(server) == ESP_OK) {
            server = nullptr;
        } else {
            ESP_LOGE(TAG, "Failed to stop http server");
        }
    }
}

// 静态成员实现
void WSServer::ws_async_send(void *arg) {
    static const char *data = "Async data";
    async_resp_arg *resp_arg = (async_resp_arg *)arg;
    httpd_handle_t hd = resp_arg->hd;
    int fd = resp_arg->fd;
    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = (uint8_t *)data;
    ws_pkt.len = strlen(data);
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
    httpd_ws_send_frame_async(hd, fd, &ws_pkt);
    free(resp_arg);
}

esp_err_t WSServer::trigger_async_send(httpd_handle_t handle, httpd_req_t *req) {
    async_resp_arg *resp_arg = (async_resp_arg *)malloc(sizeof(async_resp_arg));
    if (resp_arg == NULL) {
        return ESP_ERR_NO_MEM;
    }
    resp_arg->hd = req->handle;
    resp_arg->fd = httpd_req_to_sockfd(req);
    esp_err_t ret = httpd_queue_work(handle, ws_async_send, resp_arg);
    if (ret != ESP_OK) {
        free(resp_arg);
    }
    return ret;
}

esp_err_t WSServer::echo_handler(httpd_req_t *req) {
    if (req->method == HTTP_GET) {
        ESP_LOGI(TAG, "Handshake done, the new connection was opened");
        return ESP_OK;
    }

    httpd_ws_frame_t ws_pkt;
    uint8_t *buf = NULL;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;

    // 获取 WebSocket 帧长度
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "httpd_ws_recv_frame failed to get frame len with %d", ret);
        return ret;
    }

    ESP_LOGI(TAG, "frame len is %d", ws_pkt.len);
    if (ws_pkt.len) {
        buf = (uint8_t *)calloc(1, ws_pkt.len + 1); // 为 NULL 终止符分配空间
        if (buf == NULL) {
            ESP_LOGE(TAG, "Failed to calloc memory for buf");
            return ESP_ERR_NO_MEM;
        }

        ws_pkt.payload = (uint8_t *)buf;
        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
            free(buf);
            return ret;
        }

        ESP_LOGI(TAG, "Got packet with message: %s", ws_pkt.payload);

        std::string response;
        // 处理 WebSocket 消息
        handle_ws_message(reinterpret_cast<const char *>(ws_pkt.payload), response);

        // 发送响应
        httpd_ws_frame_t response_pkt;
        memset(&response_pkt, 0, sizeof(httpd_ws_frame_t));
        response_pkt.type = HTTPD_WS_TYPE_TEXT;
        response_pkt.payload = (uint8_t *)response.c_str();
        response_pkt.len = response.length();

        ret = httpd_ws_send_frame(req, &response_pkt);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", ret);
        }
    }

    free(buf);
    return ret;
}

httpd_handle_t WSServer::start_webserver() {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Registering the ws handler
        ESP_LOGI(TAG, "Registering URI handlers");
        static const httpd_uri_t ws = {.uri = "/ws",
                                       .method = HTTP_GET,
                                       .handler = echo_handler,
                                       .user_ctx = NULL,
                                       .is_websocket = true};
        httpd_register_uri_handler(server, &ws);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

esp_err_t WSServer::stop_webserver(httpd_handle_t server) {
    // Stop the httpd server
    return httpd_stop(server);
}

static FilamentManager filamentManager;

void handle_ws_message(const char *message, std::string &response) {
    cJSON *root = cJSON_Parse(message);
    if (!root) {
        response = R"({"error": "Invalid JSON"})";
        return;
    }
    cJSON *type = cJSON_GetObjectItem(root, "type");
    if (!cJSON_IsString(type)) {
        response = R"({"error": "Missing or invalid action"})";
        cJSON_Delete(root);
        return;
    }

    auto type_char = type->valuestring;
    if (strcmp(type_char, "setting") == 0) {
        // Handle settings actions here
        cJSON *key = cJSON_GetObjectItem(root, "key");
        if (!cJSON_IsString(key)) {
            response = R"({"error": "Missing or invalid key"})";
            cJSON_Delete(root);
            return;
        }
        auto key_char = key->valuestring;
        if (strcmp(key_char, "wifi_ssid") == 0) {
            cJSON *value = cJSON_GetObjectItem(root, "value");
            if (cJSON_IsString(value)) {
                // Save WiFi SSID to NVS or appropriate storage
                response = R"({"success": true})";
            } else {
                response = R"({"error": "Invalid value for wifi_ssid"})";
            }
        } else if (strcmp(key_char, "wifi_password") == 0) {
            cJSON *value = cJSON_GetObjectItem(root, "value");
            if (cJSON_IsString(value)) {
                // Save WiFi password to NVS or appropriate storage
                response = R"({"success": true})";
            } else {
                response = R"({"error": "Invalid value for wifi_password"})";
            }
        } else {
            response = R"({"error": "Unknown setting key"})";
        }

    } else if (strcmp(type_char, "system") == 0) {
        // 针对系统操作的处理
        cJSON *action = cJSON_GetObjectItem(root, "action");
        if (!cJSON_IsString(action)) {
            response = R"({"error": "Missing or invalid action"})";
            cJSON_Delete(root);
            return;
        }

        const char *action_char = action->valuestring;
        if (strcmp(action_char, "reboot") == 0) {
            response = R"({"success": true, "message": "Rebooting..."})";
            esp_restart();
        } else if (strcmp(action_char, "reconnect_wifi") == 0) {
            // 调用 WiFiManager 的 reconnect 方法
            if (Instance::get().wifi_manager->reconnect()) {
                response = R"({"success": true, "message": "Reconnecting to WiFi..."})";
            } else {
                response = R"({"error": "Failed to initiate WiFi reconnection"})";
            }
        } else if (strcmp(action_char, "get_mac") == 0) {
            uint8_t mac[6];
            esp_read_mac(mac, ESP_MAC_WIFI_STA);
            char mac_str[18];
            snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1],
                     mac[2], mac[3], mac[4], mac[5]);
            response = std::string(R"({"success": true, "mac": ")") + mac_str + R"("})";
        } else {
            response = R"({"error": "Unknown action"})";
        }
    } else if (strcmp(type_char, "filament") == 0) {
        cJSON *action = cJSON_GetObjectItem(root, "action");
        if (!cJSON_IsString(action)) {
            response = R"({"error": "Missing or invalid action"})";
            cJSON_Delete(root);
            return;
        }

        const char *action_char = action->valuestring;
        if (strcmp(action_char, "add") == 0) {
            cJSON *motor_id = cJSON_GetObjectItem(root, "motor_id");
            cJSON *metadata = cJSON_GetObjectItem(root, "metadata");
            if (cJSON_IsNumber(motor_id) && cJSON_IsString(metadata)) {
                int id = filamentManager.addFilament(motor_id->valueint, metadata->valuestring);
                if (id != -1) {
                    response = R"({"success": true, "id": )" + std::to_string(id) + "}";
                } else {
                    response = R"({"error": "Motor ID already in use"})";
                }
            } else {
                response = R"({"error": "Invalid parameters"})";
            }
        } else if (strcmp(action_char, "remove") == 0) {
            cJSON *id = cJSON_GetObjectItem(root, "id");
            if (cJSON_IsNumber(id)) {
                bool success = filamentManager.removeFilament(id->valueint);
                response = success ? R"({"success": true})" : R"({"error": "ID not found"})";
            } else {
                response = R"({"error": "Invalid parameters"})";
            }
        } else if (strcmp(action_char, "update") == 0) {
            cJSON *id = cJSON_GetObjectItem(root, "id");
            cJSON *motor_id = cJSON_GetObjectItem(root, "motor_id");
            cJSON *metadata = cJSON_GetObjectItem(root, "metadata");
            if (cJSON_IsNumber(id)) {
                bool success = filamentManager.updateFilament(
                    id->valueint, cJSON_IsNumber(motor_id) ? motor_id->valueint : -1,
                    cJSON_IsString(metadata) ? metadata->valuestring : "");
                response = success ? R"({"success": true})" : R"({"error": "Update failed"})";
            } else {
                response = R"({"error": "Invalid parameters"})";
            }
        } else if (strcmp(action_char, "list") == 0) {
            cJSON *id = cJSON_GetObjectItem(root, "id");
            if (cJSON_IsNumber(id)) {
                const Filament *filament = filamentManager.getFilamentById(id->valueint);
                if (filament) {
                    cJSON *filament_json = cJSON_CreateObject();
                    cJSON_AddNumberToObject(filament_json, "id", filament->id);
                    cJSON_AddNumberToObject(filament_json, "motor_id", filament->motor_id);
                    cJSON_AddStringToObject(filament_json, "metadata", filament->metadata);
                    char *json_str = cJSON_Print(filament_json);
                    response = json_str;
                    cJSON_free(json_str);
                    cJSON_Delete(filament_json);
                } else {
                    response = R"({"error": "ID not found"})";
                }
            } else {
                response = R"({"error": "Invalid parameters"})";
            }
        } else {
            response = R"({"error": "Unknown action"})";
        }
    } else if (false) {

    } else {
        response = R"({"error": "Unknown type"})";
    }

    cJSON_Delete(root);
}
