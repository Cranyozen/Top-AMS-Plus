#pragma once

#include "esp_wifi_types_generic.h"

#ifdef __cplusplus
extern "C" {
#endif

void Wifi_init(void);
void Wifi_start();
void Wifi_set_mode(wifi_mode_t mode);

#ifdef __cplusplus
}
#endif