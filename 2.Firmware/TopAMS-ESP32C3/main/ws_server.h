#pragma once
#include <esp_http_server.h>
#include <esp_event.h>
#include <esp_log.h>

class WSServer {
public:
    WSServer();
    ~WSServer();

    esp_err_t start();
    esp_err_t stop();
    httpd_handle_t getHandle() const;

    // 事件处理
    void onConnect(esp_event_base_t event_base, int32_t event_id, void* event_data);
    void onDisconnect(esp_event_base_t event_base, int32_t event_id, void* event_data);

private:
    httpd_handle_t server;
    static const char *TAG;

    struct async_resp_arg {
        httpd_handle_t hd;
        int fd;
    };

    static void ws_async_send(void *arg);
    static esp_err_t trigger_async_send(httpd_handle_t handle, httpd_req_t *req);
    static esp_err_t echo_handler(httpd_req_t *req);
    static esp_err_t stop_webserver(httpd_handle_t server);
    static httpd_handle_t start_webserver();
};
