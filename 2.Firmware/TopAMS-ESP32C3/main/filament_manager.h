#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "model/filament.h"

/**
 * @brief 3D打印耗材管理类
 */
class FilamentManager {
private:
    std::vector<Filament> filaments;           // 耗材列表
    std::map<int, size_t> id_to_index;        // ID到索引的映射，用于快速查找
    int next_id;                              // 下一个可用的ID

public:
    FilamentManager();
    ~FilamentManager();

    void init();

    int addFilament(int motor_id, const char* metadata = "{}");
    bool removeFilament(int id);
    bool updateFilament(int id, int motor_id = -1, const char* metadata = "");
    const Filament* getFilamentById(int id) const;
    const Filament* getFilamentByMotorId(int motor_id) const;
    const std::vector<Filament>& getAllFilaments() const;
    std::vector<const Filament*> findFilamentsByMetadata(const char* key, const char* value) const;
    size_t getCount() const;
    void clear();
    const char* toJson() const;
    bool fromJson(const char* json_string);

private:
    int generateId();
    void updateIndexMapping();
    bool loadFromStorage();
    bool saveToStorage() const;

    const char* nvs_key = "filaments";
};
