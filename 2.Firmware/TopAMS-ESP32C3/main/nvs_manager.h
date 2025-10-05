// nvs_manager.h
#pragma once

#include <nvs_flash.h>
#include <nvs.h>
#include <cstring>
#include <vector>
#include <type_traits>
#include <iostream>
#include "esp_log.h"

#define DEFAULT_NAMESPACE "storage"

#define NVS_TAG "[NVSManager]"

class NVSManager {
private:
    nvs_handle_t nvs_handle;
    const char* namespace_name;
    bool is_initialized;

public:
    NVSManager() : nvs_handle(0), namespace_name(DEFAULT_NAMESPACE), is_initialized(false) {}

    ~NVSManager() {
        if (is_initialized) {
            nvs_close(nvs_handle);
        }
    }

    /**
     * @brief 初始化 NVS
     * @param ns 命名空间名称
     * @return true 成功, false 失败
     */
    esp_err_t init() {

        // 初始化 NVS flash
        esp_err_t err = nvs_flash_init();
        if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            // NVS 分区已满或版本不匹配，擦除并重新初始化
            ESP_ERROR_CHECK(nvs_flash_erase());
            err = nvs_flash_init();
        }
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "NVS flash init failed: %s", esp_err_to_name(err));
            return err;
        }

        // 打开 NVS 命名空间
        err = nvs_open(namespace_name, NVS_READWRITE, &nvs_handle);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "NVS open failed for namespace '%s': %s", namespace_name, esp_err_to_name(err));
            return err;
        }

        is_initialized = true;
        ESP_LOGI(NVS_TAG, "NVS initialized and namespace '%s' opened", namespace_name);
        return err;
    }

    /**
     * @brief 保存数据到 NVS (通用模板方法)
     * @tparam T 数据类型
     * @param key 键名
     * @param value 值
     * @return true 成功, false 失败
     */
    template<typename T>
    esp_err_t set(const char* key, const T& value) {
        if (!is_initialized) {
            ESP_LOGE(NVS_TAG, "NVS not initialized");
            return ESP_ERR_INVALID_STATE;
        }

        esp_err_t err = ESP_OK;

        // 根据类型自动选择对应的 NVS API
        if constexpr (std::is_same_v<T, int8_t>) {
            err = nvs_set_i8(nvs_handle, key, value);
        } else if constexpr (std::is_same_v<T, uint8_t>) {
            err = nvs_set_u8(nvs_handle, key, value);
        } else if constexpr (std::is_same_v<T, int16_t>) {
            err = nvs_set_i16(nvs_handle, key, value);
        } else if constexpr (std::is_same_v<T, uint16_t>) {
            err = nvs_set_u16(nvs_handle, key, value);
        } else if constexpr (std::is_same_v<T, int32_t>) {
            err = nvs_set_i32(nvs_handle, key, value);
        } else if constexpr (std::is_same_v<T, uint32_t>) {
            err = nvs_set_u32(nvs_handle, key, value);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            err = nvs_set_i64(nvs_handle, key, value);
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            err = nvs_set_u64(nvs_handle, key, value);
        } else if constexpr (std::is_same_v<T, float>) {
            err = nvs_set_u32(nvs_handle, key, *reinterpret_cast<const uint32_t*>(&value));
        } else if constexpr (std::is_same_v<T, double>) {
            err = nvs_set_u64(nvs_handle, key, *reinterpret_cast<const uint64_t*>(&value));
        } else if constexpr (std::is_same_v<T, std::string>) {
            err = nvs_set_str(nvs_handle, key, value.c_str());
        } else if constexpr (std::is_same_v<T, const char*>) {
            err = nvs_set_str(nvs_handle, key, value);
        } else if constexpr (std::is_same_v<T, bool>) {
            err = nvs_set_u8(nvs_handle, key, value ? 1 : 0);
        } else {
            // 对于其他类型（如结构体、数组等），使用 blob 存储
            err = nvs_set_blob(nvs_handle, key, &value, sizeof(T));
        }

        if (err != ESP_OK) {
            ESP_LOGW(NVS_TAG, "NVS set failed for key '%s': %s", key, esp_err_to_name(err));
            return err;
        }

        ESP_LOGI(NVS_TAG, "NVS set successful for key '%s'", key);

        return err;
    }

    /**
     * @brief 从 NVS 读取数据 (通用模板方法)
     * @tparam T 数据类型
     * @param key 键名
     * @param value 输出值
     * @return true 成功, false 失败
     */
    template<typename T>
    esp_err_t get(const char* key, T& value) {
        if (!is_initialized) {
            ESP_LOGE(NVS_TAG, "NVS not initialized");
            return ESP_ERR_INVALID_STATE;
        }

        esp_err_t err = ESP_OK;

        // 根据类型自动选择对应的 NVS API
        if constexpr (std::is_same_v<T, int8_t>) {
            err = nvs_get_i8(nvs_handle, key, &value);
        } else if constexpr (std::is_same_v<T, uint8_t>) {
            err = nvs_get_u8(nvs_handle, key, &value);
        } else if constexpr (std::is_same_v<T, int16_t>) {
            err = nvs_get_i16(nvs_handle, key, &value);
        } else if constexpr (std::is_same_v<T, uint16_t>) {
            err = nvs_get_u16(nvs_handle, key, &value);
        } else if constexpr (std::is_same_v<T, int32_t>) {
            err = nvs_get_i32(nvs_handle, key, &value);
        } else if constexpr (std::is_same_v<T, uint32_t>) {
            err = nvs_get_u32(nvs_handle, key, &value);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            err = nvs_get_i64(nvs_handle, key, &value);
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            err = nvs_get_u64(nvs_handle, key, &value);
        } else if constexpr (std::is_same_v<T, float>) {
            uint32_t temp;
            err = nvs_get_u32(nvs_handle, key, &temp);
            if (err == ESP_OK) {
                value = *reinterpret_cast<float*>(&temp);
            }
        } else if constexpr (std::is_same_v<T, double>) {
            uint64_t temp;
            err = nvs_get_u64(nvs_handle, key, &temp);
            if (err == ESP_OK) {
                value = *reinterpret_cast<double*>(&temp);
            }
        } else if constexpr (std::is_same_v<T, const char*>) {
            size_t required_size = 0;
            err = nvs_get_str(nvs_handle, key, nullptr, &required_size);
            if (err == ESP_OK && required_size > 0) {
                char* buffer = new char[required_size];
                err = nvs_get_str(nvs_handle, key, buffer, &required_size);
                if (err == ESP_OK) {
                    value = buffer; // 注意：调用者需要负责释放内存
                } else {
                    delete[] buffer;
                }
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            size_t required_size = 0;
            err = nvs_get_str(nvs_handle, key, nullptr, &required_size);
            if (err == ESP_OK && required_size > 0) {
                std::vector<char> buffer(required_size);
                err = nvs_get_str(nvs_handle, key, buffer.data(), &required_size);
                if (err == ESP_OK) {
                    value = std::string(buffer.data());
                }
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            uint8_t temp;
            err = nvs_get_u8(nvs_handle, key, &temp);
            if (err == ESP_OK) {
                value = (temp != 0);
            }
        } else {
            // 对于其他类型（如结构体、数组等），使用 blob 读取
            size_t required_size = sizeof(T);
            err = nvs_get_blob(nvs_handle, key, &value, &required_size);
        }

        if (err != ESP_OK) {
            if (err == ESP_ERR_NVS_NOT_FOUND) {
                ESP_LOGW(NVS_TAG, "NVS key '%s' not found", key);
            } else {
                ESP_LOGE(NVS_TAG, "NVS get failed for key '%s': %s", key, esp_err_to_name(err));
            }
            return err;
        }

        return ESP_OK;
    }

    /**
     * @brief 提交更改到 NVS
     * @return esp_err_t 错误码
     */
    esp_err_t commit() {
        if (!is_initialized) {
            ESP_LOGE(NVS_TAG, "NVS not initialized");
            return ESP_ERR_INVALID_STATE;
        }

        esp_err_t err = nvs_commit(nvs_handle);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "NVS commit failed: %s", esp_err_to_name(err));
        }
        ESP_LOGI(NVS_TAG, "NVS commit successful");
        return err;
    }

    /**
     * @brief 删除指定键
     * @param key 键名
     * @return true 成功, false 失败
     */
    esp_err_t erase(const char* key) {
        if (!is_initialized) {
            ESP_LOGE(NVS_TAG, "NVS not initialized");
            return ESP_ERR_INVALID_STATE;
        }

        esp_err_t err = nvs_erase_key(nvs_handle, key);
        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGE(NVS_TAG, "NVS erase failed for key '%s': %s", key, esp_err_to_name(err));
            return err;
        }

        err = nvs_commit(nvs_handle);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "NVS commit failed: %s", esp_err_to_name(err));
            return err;
        }

        return err;
    }

    /**
     * @brief 清空所有数据
     * @return esp_err_t 错误码
     */
    esp_err_t clear() {
        if (!is_initialized) {
            ESP_LOGE(NVS_TAG, "NVS not initialized");
            return ESP_ERR_INVALID_STATE;
        }

        esp_err_t err = nvs_erase_all(nvs_handle);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "NVS erase all failed: %s", esp_err_to_name(err));
            return err;
        }

        err = nvs_commit(nvs_handle);
        if (err != ESP_OK) {
            ESP_LOGE(NVS_TAG, "NVS commit failed: %s", esp_err_to_name(err));
            return err;
        }

        return ESP_OK;
    }

    /**
     * @brief 检查键是否存在
     * @param key 键名
     * @return true 存在, false 不存在
     */
    bool exists(const char* key) {
        if (!is_initialized) {
            ESP_LOGE(NVS_TAG, "NVS not initialized");
            return false;
        }

        // 尝试获取值的大小来检查键是否存在
        esp_err_t err = nvs_get_u32(nvs_handle, key, nullptr);
        return (err == ESP_OK || err == ESP_ERR_NVS_INVALID_LENGTH);
    }
};
