#include <string>
#include "cJSON.h"

/**
 * @brief 3D打印耗材结构体
 */
struct Filament {
    int id;                    // 唯一标识符
    int motor_id;             // 电机编号
    const char* metadata;     // 元数据（JSON字符串格式）
    
    // 构造函数
    Filament() : id(0), motor_id(0) {}
    
    Filament(int _id, int _motor_id, const char* _metadata) 
        : id(_id), motor_id(_motor_id), metadata(_metadata) {}
    
    // 拷贝构造函数
    Filament(const Filament& other) 
        : id(other.id), motor_id(other.motor_id), metadata(other.metadata) {}
    
    // 赋值操作符
    Filament& operator=(const Filament& other) {
        if (this != &other) {
            id = other.id;
            motor_id = other.motor_id;
            metadata = other.metadata;
        }
        return *this;
    }
    
    /**
     * @brief 将耗材结构体转换为JSON字符串
     * @return JSON字符串
     */
    std::string toJson() const {
        cJSON *json = cJSON_CreateObject();
        cJSON_AddNumberToObject(json, "id", id);
        cJSON_AddNumberToObject(json, "motor_id", motor_id);
        cJSON_AddStringToObject(json, "metadata", metadata);
        
        char *json_string = cJSON_Print(json);
        std::string result(json_string);
        cJSON_Delete(json);
        
        return result;
    }
    
    /**
     * @brief 从JSON字符串创建耗材结构体
     * @param json_string JSON字符串
     * @return true 成功, false 失败
     */
    bool fromJson(const std::string& json_string) {
        cJSON *json = cJSON_Parse(json_string.c_str());
        if (json == nullptr) {
            return false;
        }
        
        cJSON *id_obj = cJSON_GetObjectItemCaseSensitive(json, "id");
        cJSON *motor_id_obj = cJSON_GetObjectItemCaseSensitive(json, "motor_id");
        cJSON *metadata_obj = cJSON_GetObjectItemCaseSensitive(json, "metadata");
        
        bool success = true;
        if (cJSON_IsNumber(id_obj)) {
            id = id_obj->valueint;
        } else {
            success = false;
        }
        
        if (cJSON_IsNumber(motor_id_obj)) {
            motor_id = motor_id_obj->valueint;
        } else {
            success = false;
        }
        
        if (cJSON_IsString(metadata_obj) && metadata_obj->valuestring != nullptr) {
            metadata = metadata_obj->valuestring;
        } else {
            success = false;
        }
        
        cJSON_Delete(json);
        return success;
    }
    
    /**
     * @brief 获取元数据中的特定字段值
     * @param key 字段名
     * @return 字段值，如果不存在返回空字符串
     */
    std::string getMetadataValue(const char* key) const {
        cJSON *json = cJSON_Parse(metadata);
        if (json == nullptr) {
            return "";
        }
        
        cJSON *item = cJSON_GetObjectItemCaseSensitive(json, key);
        std::string result = "";
        
        if (cJSON_IsString(item) && item->valuestring != nullptr) {
            result = std::string(item->valuestring);
        } else if (cJSON_IsNumber(item)) {
            result = std::to_string(item->valuedouble);
        } else if (cJSON_IsBool(item)) {
            result = cJSON_IsTrue(item) ? "true" : "false";
        }
        
        cJSON_Delete(json);
        return result;
    }
    
    /**
     * @brief 设置元数据中的特定字段值
     * @param key 字段名
     * @param value 字段值
     */
    void setMetadataValue(const char* key, const char* value) {
        cJSON *json = nullptr;
        
        // 如果metadata不为空且是有效的JSON，解析它
        if (metadata != nullptr && metadata[0] != '\0') {
            json = cJSON_Parse(metadata);
        }
        
        // 如果解析失败或为空，创建新的JSON对象
        if (json == nullptr) {
            json = cJSON_CreateObject();
        }
        
        // 设置字段值
        cJSON_DeleteItemFromObject(json, key);
        cJSON_AddStringToObject(json, key, value);
        
        // 转换回字符串
        char *json_string = cJSON_Print(json);
        metadata = json_string;
        cJSON_Delete(json);
    }
};
