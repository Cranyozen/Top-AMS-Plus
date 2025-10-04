/*
    From ha-bambulab
    https://github.com/greghesp/ha-bambulab/blob/main/custom_components/bambu_lab/pybambu/commands.py
*/

#pragma once

// 带参数模板的格式化函数
#include <string>
#include <sstream>

namespace BambuCmd
{
    /*
    """MQTT Commands"""
CHAMBER_LIGHT_ON = {
    "system": {"sequence_id": "0", "command": "ledctrl", "led_node": "chamber_light", "led_mode": "on",
               "led_on_time": 500, "led_off_time": 500, "loop_times": 0, "interval_time": 0}}
CHAMBER_LIGHT_OFF = {
    "system": {"sequence_id": "0", "command": "ledctrl", "led_node": "chamber_light", "led_mode": "off",
               "led_on_time": 500, "led_off_time": 500, "loop_times": 0, "interval_time": 0}}
CHAMBER_LIGHT_2_ON = {
    "system": {"sequence_id": "0", "command": "ledctrl", "led_node": "chamber_light2", "led_mode": "on",
               "led_on_time": 500, "led_off_time": 500, "loop_times": 0, "interval_time": 0}}
CHAMBER_LIGHT_2_OFF = {
    "system": {"sequence_id": "0", "command": "ledctrl", "led_node": "chamber_light2", "led_mode": "off",
               "led_on_time": 500, "led_off_time": 500, "loop_times": 0, "interval_time": 0}}

HEATBED_LIGHT_ON = {
    "system": {"sequence_id": "0", "command": "ledctrl", "led_node": "heatbed_light", "led_mode": "on",
               "led_on_time": 0, "led_off_time": 0, "loop_times": 0, "interval_time": 0}}
HEATBED_LIGHT_OFF = {
    "system": {"sequence_id": "0", "command": "ledctrl", "led_node": "heatbed_light", "led_mode": "off",
               "led_on_time": 0, "led_off_time": 0, "loop_times": 0, "interval_time": 0}}

SPEED_PROFILE_TEMPLATE = {"print": {"sequence_id": "0", "command": "print_speed", "param": ""}}

GET_VERSION = {"info": {"sequence_id": "0", "command": "get_version"}}

PAUSE = {"print": {"sequence_id": "0", "command": "pause"}}
RESUME = {"print": {"sequence_id": "0", "command": "resume"}}
STOP = {"print": {"sequence_id": "0", "command": "stop"}}

PUSH_ALL = {"pushing": {"sequence_id": "0", "command": "pushall"}}

START_PUSH = { "pushing": {"sequence_id": "0", "command": "start"}}

SEND_GCODE_TEMPLATE = {
    "print": {
        "sequence_id": "0",
        "command": "gcode_line",
        "param": "" # param = GCODE_EACH_LINE_SEPARATED_BY_\n
    }
}

UPGRADE_CONFIRM_TEMPLATE = {
    "upgrade": {
        "command": "upgrade_confirm",
        "module": "ota",
        "reason": "",
        "result": "success",
        "sequence_id": "0",
        "src_id": 2,
        "upgrade_type": 4,
        "url": "https://public-cdn.bblmw.com/upgrade/device/{model}/{version}/product/{hash}/{stamp}.json.sig",
        "version": "{version}",
    }
}

PRINT_PROJECT_FILE_TEMPLATE = {
    "print": {
        "sequence_id": 0,
        "command": "project_file",

        "param": "", # param = f"Metadata/plate_1.gcode"
        "url": "", # url = f"ftp://{file}"
        "bed_type": "auto",
        "timelapse": False,
        "bed_leveling": True,
        "flow_cali": True,
        "vibration_cali": True,
        "layer_inspect": True,
        "use_ams": False,
        "ams_mapping": [0],

        "subtask_name": "",
        "profile_id": "0",
        "project_id": "0",
        "subtask_id": "0",
        "task_id": "0",
    }
}

SKIP_OBJECTS_TEMPLATE = {
    "print": {
        "sequence_id": "0",
        "command": "skip_objects",
        "obj_list": []
    }
}

SWITCH_AMS_TEMPLATE = {
    "print": {
        "command": "ams_change_filament",
        "sequence_id": "0",
        "target": 255,
        "curr_temp": 0,
        "tar_temp": 0
    }
}

AMS_FILAMENT_SETTING_TEMPLATE = {
    "print": {
        "sequence_id": "0",
        "command": "ams_filament_setting",
        "ams_id": 0,                # Index of the AMS
        "tray_id": 0,               # Index of the tray with the AMS
        "tray_info_idx": "",        # The setting ID of the filament profile
        "tray_color": "000000FF",   # Formatted as hex RRGGBBAA (alpha is always FF)
        "nozzle_temp_min": 0,       # Minimum nozzle temp for filament (in C)
        "nozzle_temp_max": 0,       # Maximum nozzle temp for filament (in C)
        "tray_type": "PLA"          # Type of filament, such as "PLA" or "ABS"
    }
}

MOVE_AXIS_GCODE = "M211 S\nM211 X1 Y1 Z1\nM1002 push_ref_mode\nG91 \nG1 {axis}{distance}.0 F{speed}\nM1002 pop_ref_mode\nM211 R\n"
HOME_GCODE = "G28\n"
EXTRUDER_GCODE = "M83 \nG0 E{distance}.0 F900\n"

# X1 only currently
GET_ACCESSORIES = {"system": {"sequence_id": "0", "command": "get_accessories", "accessory_type": "none"}}

# A1 and H2D only
PROMPT_SOUND_ENABLE  = {"print" : {"sequence_id": "0", "command": "print_option", "sound_enable": True}}
PROMPT_SOUND_DISABLE = {"print" : {"sequence_id": "0", "command": "print_option", "sound_enable": False}}

# H2D only
BUZZER_SET_SILENT  = {"print" : {"sequence_id": "0", "command": "buzzer_ctrl", "mode": 0, "reason": ""}}
BUZZER_SET_ALARM   = {"print" : {"sequence_id": "0", "command": "buzzer_ctrl", "mode": 1, "reason": ""}}
BUZZER_SET_BEEPING = {"print" : {"sequence_id": "0", "command": "buzzer_ctrl", "mode": 2, "reason": ""}}*/

    constexpr const char* CHAMBER_LIGHT_ON = R"({"system":{"sequence_id":"0","command":"ledctrl","led_node":"chamber_light","led_mode":"on","led_on_time":500,"led_off_time":500,"loop_times":0,"interval_time":0}})";
    constexpr const char* CHAMBER_LIGHT_OFF = R"({"system":{"sequence_id":"0","command":"ledctrl","led_node":"chamber_light","led_mode":"off","led_on_time":500,"led_off_time":500,"loop_times":0,"interval_time":0}})";
    constexpr const char* CHAMBER_LIGHT_2_ON = R"({"system":{"sequence_id":"0","command":"ledctrl","led_node":"chamber_light2","led_mode":"on","led_on_time":500,"led_off_time":500,"loop_times":0,"interval_time":0}})";
    constexpr const char* CHAMBER_LIGHT_2_OFF = R"({"system":{"sequence_id":"0","command":"ledctrl","led_node":"chamber_light2","led_mode":"off","led_on_time":500,"led_off_time":500,"loop_times":0,"interval_time":0}})";

    constexpr const char* HEATBED_LIGHT_ON = R"({"system":{"sequence_id":"0","command":"ledctrl","led_node":"heatbed_light","led_mode":"on","led_on_time":0,"led_off_time":0,"loop_times":0,"interval_time":0}})";
    constexpr const char* HEATBED_LIGHT_OFF = R"({"system":{"sequence_id":"0","command":"ledctrl","led_node":"heatbed_light","led_mode":"off","led_on_time":0,"led_off_time":0,"loop_times":0,"interval_time":0}})";

    constexpr const char* SPEED_PROFILE_TEMPLATE = R"({"print":{"sequence_id":"0","command":"print_speed","param":""}})";

    constexpr const char* GET_VERSION = R"({"info":{"sequence_id":"0","command":"get_version"}})";

    constexpr const char* PAUSE = R"({"print":{"sequence_id":"0","command":"pause"}})";
    constexpr const char* RESUME = R"({"print":{"sequence_id":"0","command":"resume"}})";
    constexpr const char* STOP = R"({"print":{"sequence_id":"0","command":"stop"}})";

    constexpr const char* PUSH_ALL = R"({"pushing":{"sequence_id":"0","command":"pushall"}})";
    constexpr const char* START_PUSH = R"({"pushing":{"sequence_id":"0","command":"start"}})";

    constexpr const char* SEND_GCODE_TEMPLATE = R"({"print":{"sequence_id":"0","command":"gcode_line","param":""}})";

    // constexpr const char* UPGRADE_CONFIRM_TEMPLATE = R"({"upgrade":{"command":"upgrade_confirm","module":"ota","reason":"","result":"success","sequence_id":"0","src_id":2,"upgrade_type":4,"url":"https://public-cdn.bblmw.com/upgrade/device/{model}/{version}/product/{hash}/{stamp}.json.sig","version":"{version}"}})";

    // constexpr const char* PRINT_PROJECT_FILE_TEMPLATE = R"({"print":{"sequence_id":0,"command":"project_file","param":"","url":"","bed_type":"auto","timelapse":false,"bed_leveling":true,"flow_cali":true,"vibration_cali":true,"layer_inspect":true,"use_ams":false,"ams_mapping":[0],"subtask_name":"","profile_id":"0","project_id":"0","subtask_id":"0","task_id":"0"}})";

    // constexpr const char* SKIP_OBJECTS_TEMPLATE = R"({"print":{"sequence_id":"0","command":"skip_objects","obj_list":[]}})";

    constexpr const char* SWITCH_AMS_TEMPLATE = R"({"print":{"command":"ams_change_filament","sequence_id":"0","target":255,"curr_temp":0,"tar_temp":0}})";

    constexpr const char* AMS_FILAMENT_SETTING_TEMPLATE = R"({"print":{"sequence_id":"0","command":"ams_filament_setting","ams_id":0,"tray_id":0,"tray_info_idx":"","tray_color":"000000FF","nozzle_temp_min":0,"nozzle_temp_max":0,"tray_type":"PLA"}})";

    constexpr const char* MOVE_AXIS_GCODE = "M211 S\nM211 X1 Y1 Z1\nM1002 push_ref_mode\nG91 \nG1 {axis}{distance}.0 F{speed}\nM1002 pop_ref_mode\nM211 R\n";
    constexpr const char* HOME_GCODE = "G28\n";
    constexpr const char* EXTRUDER_GCODE = "M83 \nG0 E{distance}.0 F900\n";

    // X1 only currently
    constexpr const char* GET_ACCESSORIES = R"({"system":{"sequence_id":"0","command":"get_accessories","accessory_type":"none"}})";

    // A1 and H2D only
    constexpr const char* PROMPT_SOUND_ENABLE = R"({"print":{"sequence_id":"0","command":"print_option","sound_enable":true}})";
    constexpr const char* PROMPT_SOUND_DISABLE = R"({"print":{"sequence_id":"0","command":"print_option","sound_enable":false}})";

    // H2D only
    constexpr const char* BUZZER_SET_SILENT = R"({"print":{"sequence_id":"0","command":"buzzer_ctrl","mode":0,"reason":""}})";
    constexpr const char* BUZZER_SET_ALARM = R"({"print":{"sequence_id":"0","command":"buzzer_ctrl","mode":1,"reason":""}})";
    constexpr const char* BUZZER_SET_BEEPING = R"({"print":{"sequence_id":"0","command":"buzzer_ctrl","mode":2,"reason":""}})";

    // SPEED_PROFILE_TEMPLATE: param
    inline std::string SpeedProfileCmd(const std::string& param) {
        std::ostringstream oss;
        oss << R"({"print":{"sequence_id":"0","command":"print_speed","param":")" << param << R"("}})";
        return oss.str();
    }

    // SEND_GCODE_TEMPLATE: param
    inline std::string SendGcodeCmd(const std::string& gcode) {
        std::ostringstream oss;
        oss << R"({"print":{"sequence_id":"0","command":"gcode_line","param":")" << gcode << R"("}})";
        return oss.str();
    }

    // // UPGRADE_CONFIRM_TEMPLATE: model, version, hash, stamp
    // inline std::string UpgradeConfirmCmd(const std::string& model, const std::string& version, const std::string& hash, const std::string& stamp) {
    //     std::ostringstream oss;
    //     oss << R"({"upgrade":{"command":"upgrade_confirm","module":"ota","reason":"","result":"success","sequence_id":"0","src_id":2,"upgrade_type":4,"url":"https://public-cdn.bblmw.com/upgrade/device/)"
    //         << model << "/" << version << "/product/" << hash << "/" << stamp << R"(.json.sig","version":")" << version << R"("}})";
    //     return oss.str();
    // }

    // // PRINT_PROJECT_FILE_TEMPLATE: param, url, bed_type, timelapse, bed_leveling, flow_cali, vibration_cali, layer_inspect, use_ams, ams_mapping, subtask_name, profile_id, project_id, subtask_id, task_id
    // inline std::string PrintProjectFileCmd(
    //     const std::string& param,
    //     const std::string& url,
    //     const std::string& bed_type = "auto",
    //     bool timelapse = false,
    //     bool bed_leveling = true,
    //     bool flow_cali = true,
    //     bool vibration_cali = true,
    //     bool layer_inspect = true,
    //     bool use_ams = false,
    //     const std::string& ams_mapping = "[0]",
    //     const std::string& subtask_name = "",
    //     const std::string& profile_id = "0",
    //     const std::string& project_id = "0",
    //     const std::string& subtask_id = "0",
    //     const std::string& task_id = "0"
    // ) {
    //     std::ostringstream oss;
    //     oss << R"({"print":{"sequence_id":0,"command":"project_file","param":")" << param
    //         << R"(","url":")" << url
    //         << R"(","bed_type":")" << bed_type
    //         << R"(","timelapse":)" << (timelapse ? "true" : "false")
    //         << R"(,"bed_leveling":)" << (bed_leveling ? "true" : "false")
    //         << R"(,"flow_cali":)" << (flow_cali ? "true" : "false")
    //         << R"(,"vibration_cali":)" << (vibration_cali ? "true" : "false")
    //         << R"(,"layer_inspect":)" << (layer_inspect ? "true" : "false")
    //         << R"(,"use_ams":)" << (use_ams ? "true" : "false")
    //         << R"(,"ams_mapping":)" << ams_mapping
    //         << R"(,"subtask_name":")" << subtask_name
    //         << R"(","profile_id":")" << profile_id
    //         << R"(","project_id":")" << project_id
    //         << R"(","subtask_id":")" << subtask_id
    //         << R"(","task_id":")" << task_id
    //         << R"("}})";
    //     return oss.str();
    // }

    // // SKIP_OBJECTS_TEMPLATE: obj_list (json array string)
    // inline std::string SkipObjectsCmd(const std::string& obj_list_json) {
    //     std::ostringstream oss;
    //     oss << R"({"print":{"sequence_id":"0","command":"skip_objects","obj_list":)" << obj_list_json << "}}";
    //     return oss.str();
    // }

    // SWITCH_AMS_TEMPLATE: target, curr_temp, tar_temp
    inline std::string SwitchAmsCmd(int target, int curr_temp, int tar_temp) {
        std::ostringstream oss;
        oss << R"({"print":{"command":"ams_change_filament","sequence_id":"0","target":)" << target
            << R"(,"curr_temp":)" << curr_temp
            << R"(,"tar_temp":)" << tar_temp
            << "}}";
        return oss.str();
    }

    // AMS_FILAMENT_SETTING_TEMPLATE: ams_id, tray_id, tray_info_idx, tray_color, nozzle_temp_min, nozzle_temp_max, tray_type
    inline std::string AmsFilamentSettingCmd(
        int ams_id,
        int tray_id,
        const std::string& tray_info_idx,
        const std::string& tray_color,
        int nozzle_temp_min,
        int nozzle_temp_max,
        const std::string& tray_type
    ) {
        std::ostringstream oss;
        oss << R"({"print":{"sequence_id":"0","command":"ams_filament_setting","ams_id":)" << ams_id
            << R"(,"tray_id":)" << tray_id
            << R"(,"tray_info_idx":")" << tray_info_idx
            << R"(","tray_color":")" << tray_color
            << R"(","nozzle_temp_min":)" << nozzle_temp_min
            << R"(,"nozzle_temp_max":)" << nozzle_temp_max
            << R"(,"tray_type":")" << tray_type
            << R"("}})";
        return oss.str();
    }

    // MOVE_AXIS_GCODE: axis, distance, speed
    inline std::string MoveAxisGcode(char axis, int distance, int speed) {
        std::ostringstream oss;
        oss << "M211 S\nM211 X1 Y1 Z1\nM1002 push_ref_mode\nG91 \nG1 "
            << axis << distance << ".0 F" << speed << "\nM1002 pop_ref_mode\nM211 R\n";
        return oss.str();
    }

    // EXTRUDER_GCODE: distance
    inline std::string ExtruderGcode(int distance) {
        std::ostringstream oss;
        oss << "M83 \nG0 E" << distance << ".0 F900\n";
        return oss.str();
    }
} // namespace BambuCmd
