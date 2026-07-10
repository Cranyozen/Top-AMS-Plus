#pragma once

#include <stddef.h>
#include "esp_err.h"
#include "nvs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	nvs_handle_t handle;
	const char *nvs_namespace;
} nvs_context_t;

esp_err_t Nvs_init(nvs_context_t *context);
esp_err_t Nvs_close(nvs_context_t *context);
esp_err_t Nvs_get_blob(nvs_context_t *context, const char *key, char *buf, size_t len);
esp_err_t Nvs_set_blob(nvs_context_t *context, const char *key, const char *buf, size_t len);
esp_err_t Nvs_commit(nvs_context_t *context);

#ifdef __cplusplus
}
#endif