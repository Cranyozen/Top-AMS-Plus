#include <algorithm>
#include "cJSON.h"

#include "esp_log.h"

#include "filament_manager.h"
#include "instance.h"

static const char *TAG = "[FilamentManager]";

FilamentManager::FilamentManager() : next_id(1) {}
FilamentManager::~FilamentManager() {
    // 保存数据到存储
    if (!saveToStorage()) {
        ESP_LOGE(TAG, "Failed to save filaments on destruction");
    }
}

void FilamentManager::init() {
    filaments.clear();
    id_to_index.clear();
    next_id = 1;

    // 从存储加载数据
    if (!loadFromStorage()) {
        ESP_LOGW(TAG, "Failed to load filaments from storage, starting fresh");
    }
}

int FilamentManager::addFilament(int motor_id, const char* metadata) {
    if (getFilamentByMotorId(motor_id) != nullptr) {
        ESP_LOGW(TAG, "Motor ID %d is already in use", motor_id);
        return -1;
    }
    int new_id = generateId();
    Filament filament(new_id, motor_id, metadata);
    filaments.push_back(filament);
    id_to_index[new_id] = filaments.size() - 1;
    saveToStorage();
    return new_id;
}

bool FilamentManager::removeFilament(int id) {
    auto it = id_to_index.find(id);
    if (it == id_to_index.end()) {
        return false;
    }
    size_t index = it->second;
    filaments.erase(filaments.begin() + index);
    id_to_index.erase(it);
    updateIndexMapping();
    return true;
}

bool FilamentManager::updateFilament(int id, int motor_id, const char* metadata) {
    auto it = id_to_index.find(id);
    if (it == id_to_index.end()) {
        return false;
    }
    size_t index = it->second;
    Filament& filament = filaments[index];
    if (motor_id != -1 && motor_id != filament.motor_id) {
        if (getFilamentByMotorId(motor_id) != nullptr) {
            ESP_LOGW(TAG, "Motor ID %d is already in use", motor_id);
            return false;
        }
        filament.motor_id = motor_id;
    }
    if (metadata != nullptr && metadata[0] != '\0') {
        filament.metadata = metadata;
    }
    saveToStorage();
    return true;
}

const Filament* FilamentManager::getFilamentById(int id) const {
    auto it = id_to_index.find(id);
    if (it == id_to_index.end()) {
        return nullptr;
    }
    size_t index = it->second;
    return &filaments[index];
}

const Filament* FilamentManager::getFilamentByMotorId(int motor_id) const {
    auto it = std::find_if(filaments.begin(), filaments.end(),
        [motor_id](const Filament& f) { return f.motor_id == motor_id; });
    if (it != filaments.end()) {
        return &(*it);
    }
    return nullptr;
}

const std::vector<Filament>& FilamentManager::getAllFilaments() const {
    return filaments;
}

std::vector<const Filament*> FilamentManager::findFilamentsByMetadata(const char* key, const char* value) const {
    std::vector<const Filament*> result;
    for (const auto& filament : filaments) {
        if (filament.getMetadataValue(key) == value) {
            result.push_back(&filament);
        }
    }
    return result;
}

size_t FilamentManager::getCount() const {
    return filaments.size();
}

void FilamentManager::clear() {
    filaments.clear();
    id_to_index.clear();
    next_id = 1;
}

const char* FilamentManager::toJson() const {
    cJSON *json_array = cJSON_CreateArray();
    for (const auto& filament : filaments) {
        cJSON *filament_json = cJSON_CreateObject();
        cJSON_AddNumberToObject(filament_json, "id", filament.id);
        cJSON_AddNumberToObject(filament_json, "motor_id", filament.motor_id);
        cJSON_AddStringToObject(filament_json, "metadata", filament.metadata);
        cJSON_AddItemToArray(json_array, filament_json);
    }
    char *json_string = cJSON_PrintUnformatted(json_array);
    // cJSON_free(json_string);
    cJSON_Delete(json_array);
    return json_string;
}

bool FilamentManager::fromJson(const char* json_string) {
    cJSON *json_array = cJSON_Parse(json_string);
    if (json_array == nullptr || !cJSON_IsArray(json_array)) {
        return false;
    }
    clear();
    int array_size = cJSON_GetArraySize(json_array);
    bool success = true;
    for (int i = 0; i < array_size; i++) {
        cJSON *item = cJSON_GetArrayItem(json_array, i);
        if (item == nullptr || !cJSON_IsObject(item)) {
            success = false;
            continue;
        }
        cJSON *id_obj = cJSON_GetObjectItemCaseSensitive(item, "id");
        cJSON *motor_id_obj = cJSON_GetObjectItemCaseSensitive(item, "motor_id");
        cJSON *metadata_obj = cJSON_GetObjectItemCaseSensitive(item, "metadata");
        if (cJSON_IsNumber(id_obj) && cJSON_IsNumber(motor_id_obj) && 
            cJSON_IsString(metadata_obj) && metadata_obj->valuestring != nullptr) {
            int id = id_obj->valueint;
            int motor_id = motor_id_obj->valueint;
            const char* metadata = metadata_obj->valuestring;
            if (id_to_index.find(id) != id_to_index.end()) {
                success = false;
                continue;
            }
            if (getFilamentByMotorId(motor_id) != nullptr) {
                success = false;
                continue;
            }
            Filament filament(id, motor_id, metadata);
            filaments.push_back(filament);
            id_to_index[id] = filaments.size() - 1;
            if (id >= next_id) {
                next_id = id + 1;
            }
        } else {
            success = false;
        }
    }
    if (!success) {
        clear();
    }
    cJSON_Delete(json_array);
    return success;
}

int FilamentManager::generateId() {
    while (id_to_index.find(next_id) != id_to_index.end()) {
        next_id++;
    }
    return next_id++;
}

void FilamentManager::updateIndexMapping() {
    id_to_index.clear();
    for (size_t i = 0; i < filaments.size(); i++) {
        id_to_index[filaments[i].id] = i;
    }
}

bool FilamentManager::loadFromStorage() {
    const char* json_data;
    esp_err_t err = Instance::get().nvs_manager->get<const char*>(nvs_key, json_data);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Loaded filaments data from storage");
        ESP_LOGI(TAG, "Filaments JSON: %s", json_data);
        return fromJson(json_data);
    }
    ESP_LOGW(TAG, "No filament data found in storage");
    return false;
}

bool FilamentManager::saveToStorage() const {
    ESP_LOGI(TAG, "Saving filaments to storage");
    const char* json_data = toJson();
    esp_err_t err = Instance::get().nvs_manager->set<const char*>(nvs_key, json_data);
    if (err == ESP_OK) {
        return Instance::get().nvs_manager->commit() == ESP_OK;
    }
    ESP_LOGE(TAG, "Failed to save filaments to storage: %s", esp_err_to_name(err));
    return false;
}
