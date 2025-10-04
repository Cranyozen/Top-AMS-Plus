/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * MDNS-SD Query and advertise Example
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_netif_ip_addr.h"
#include "esp_mac.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "mdns.h"
#include "driver/gpio.h"
#include "netdb.h"
#include "mdns_service.h"

#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 1, 0)
/* CONFIG_LWIP_IPV4 was introduced in IDF v5.1, set CONFIG_LWIP_IPV4 to 1 by default for IDF v5.0 */
#ifndef CONFIG_LWIP_IPV4
#define CONFIG_LWIP_IPV4 1
#endif // CONFIG_LWIP_IPV4
#endif // ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 1, 0)

#define EXAMPLE_MDNS_INSTANCE CONFIG_MDNS_INSTANCE
#define EXAMPLE_BUTTON_GPIO   CONFIG_MDNS_BUTTON_GPIO

const char* MDnsService::TAG = "[MDnsService]";

MDnsService::MDnsService(const std::string& instance_name, const std::string& service_name, const std::string& proto, uint16_t port)
    : instance_name_(instance_name), service_name_(service_name), proto_(proto), port_(port) {}

MDnsService::~MDnsService() {
    mdns_free();
}

void MDnsService::init() {
    ESP_ERROR_CHECK(mdns_init());
    ESP_ERROR_CHECK(mdns_hostname_set(instance_name_.c_str()));
    ESP_LOGI(TAG, "mDNS initialized with instance name: %s", instance_name_.c_str());
}

void MDnsService::addService() {
    ESP_ERROR_CHECK(mdns_service_add(service_name_.c_str(), proto_.c_str(), "_tcp", port_, nullptr, 0));
    ESP_LOGI(TAG, "Service added: %s.%s.local:%d", service_name_.c_str(), proto_.c_str(), port_);
}
