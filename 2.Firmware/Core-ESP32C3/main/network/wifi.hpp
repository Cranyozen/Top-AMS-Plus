#pragma once

#include "esp_wifi_types_generic.h"

#ifdef __cplusplus
extern "C" {
#endif

void Wifi_Init(void);
void Wifi_Start();
void Wifi_SetMode(wifi_mode_t mode);

#ifdef __cplusplus
}
#endif