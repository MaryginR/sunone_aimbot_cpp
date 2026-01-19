#define WIN32_LEAN_AND_MEAN
#define _WINSOCKAPI_
#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <filesystem>
#include <unordered_map>

#include "config.h"
#include "modules/SimpleIni.h"

std::vector<std::string> Config::splitString(const std::string& str, char delimiter)
{
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delimiter))
    {
        while (!item.empty() && (item.front() == ' ' || item.front() == '\t'))
            item.erase(item.begin());
        while (!item.empty() && (item.back() == ' ' || item.back() == '\t'))
            item.pop_back();

        tokens.push_back(item);
    }
    return tokens;
}

std::string Config::joinStrings(const std::vector<std::string>& vec, const std::string& delimiter)
{
    std::ostringstream oss;
    for (size_t i = 0; i < vec.size(); ++i)
    {
        if (i != 0) oss << delimiter;
        oss << vec[i];
    }
    return oss.str();
}

static std::vector<int> parseClassList(const std::string value)
{
    std::vector<int> result;
    std::stringstream ss(value);
    std::string token;

    while (std::getline(ss, token, ','))
    {
        try {
            int v = std::stoi(token);
            result.push_back(v);
        }
        catch (...) {}
    }

    return result;
}

static std::string joinKeyFilePairs(const std::vector<std::string>& keys, const std::vector<std::string>& files)
{
    std::ostringstream oss;
    size_t n = (((keys.size()) < (files.size())) ? (keys.size()) : (files.size()));
    for (size_t i = 0; i < n; i++) {
        if (i) oss << ",";
        oss << keys[i] << ":" << files[i];
    }
    return oss.str();
}

bool Config::loadConfig(const std::string& filename)
{
    if (!std::filesystem::exists(filename))
    {
        std::cerr << "[Config] Config file does not exist, creating default config: " << filename << std::endl;

        // Capture
        capture_method = "duplication_api";
        capture_target = "monitor";
        capture_window_title = "";
        detection_resolution = 320;
        capture_fps = 60;
        monitor_idx = 0;
        circle_mask = true;
        capture_borders = true;
        capture_cursor = true;
        virtual_camera_name = "None";
        virtual_camera_width = 1920;
        virtual_camera_heigth = 1080;

        // Target
        disable_headshot = false;
        body_y_offset = 0.15f;
        head_y_offset = 0.05f;
        auto_aim = false;

        // Mouse
        fovX = 106;
        fovY = 74;
        minSpeedMultiplier = 0.1f;
        maxSpeedMultiplier = 0.1f;

        predictionInterval = 0.01f;
        prediction_futurePositions = 20;
        draw_futurePositions = true;

        snapRadius = 1.5f;
        nearRadius = 25.0f;
        speedCurveExponent = 3.0f;
        snapBoostFactor = 1.15f;

        easynorecoil = false;
        easynorecoilstrength = 0.0f;
        easynorecoil_offsetY = 0.0f;        // Максимальное смещение по оси Y
        easynorecoil_increaseSpeed = 1.0f;  // Скорость увеличения смещения
        easynorecoil_returnSpeed = 1.0f;    // Скорость возврата
        input_method = "WIN32";

        // Wind mouse
        wind_mouse_enabled = false;
        wind_G = 18.0f;
        wind_W = 15.0f;
        wind_M = 10.0f;
        wind_D = 8.0f;
        starts_after_mouse_move = false;
        move_timeout = 10;
        min_mouse_move_length = 1;
        aim_timeout = 10;
        max_aim_distance = 1;

        // Arduino
        arduino_baudrate = 115200;
        arduino_port = "COM0";
        arduino_16_bit_mouse = false;
        arduino_enable_keys = false;

        // MIDI
        midi_device_name = "";

        // kmbox_B
        kmbox_b_baudrate = 115200;
        kmbox_b_port = "COM0";

        // kmbox_net
        kmbox_net_ip = "10.42.42.42";
        kmbox_net_port = "1984";
        kmbox_net_uuid = "DEADC0DE";

        // makcu
        makcu_baudrate = 115200;
        makcu_port = "COM0";

        // Mouse shooting
        auto_shoot = false;
        bScope_multiplier = 1.0f;

        // AI
#ifdef USE_CUDA
        backend = "TRT";
#else
        backend = "DML";
#endif
        dml_device_id = 0;

#ifdef USE_CUDA
        ai_model = "sunxds_0.5.6.engine";
#else
        ai_model = "sunxds_0.5.6.onnx";
#endif

        confidence_threshold = 0.10f;
        nms_threshold = 0.50f;
        max_detections = 100;
#ifdef USE_CUDA
        export_enable_fp8 = false;
        export_enable_fp16 = true;
#endif
        fixed_input_size = false;

        // CUDA
#ifdef USE_CUDA
        use_cuda_graph = false;
        use_pinned_memory = false;
        gpuMemoryReserveMB = 2048;
        enableGpuExclusiveMode = true;
#endif

        // System
        cpuCoreReserveCount = 4;
        systemMemoryReserveMB = 2048;

        // Buttons
        button_targeting = splitString("RightMouseButton");
        button_shoot = splitString("LeftMouseButton");
        button_zoom = splitString("RightMouseButton");
        button_exit = splitString("F2");
        button_pause = splitString("F3");
        button_reload_config = splitString("F4");
        button_open_overlay = splitString("Home");
        enable_arrows_settings = false;

        // Overlay
        overlay_opacity = 225;
        overlay_ui_scale = 1.0f;

        // Depth
        depth_model_path = "depth_anything_v2.engine";
        depth_fps = 100;

        // Game overlay
        game_overlay_enabled = false;
        game_overlay_max_fps = 0;
        game_overlay_draw_boxes = true;
        game_overlay_draw_future = true;
        game_overlay_draw_frame = true;
        game_overlay_show_target_correction = true;
        game_overlay_box_a = 255;
        game_overlay_box_r = 0;
        game_overlay_box_g = 255;
        game_overlay_box_b = 0;
        game_overlay_frame_a = 180;
        game_overlay_frame_r = 255;
        game_overlay_frame_g = 255;
        game_overlay_frame_b = 255;
        game_overlay_box_thickness = 2.0f;
        game_overlay_frame_thickness = 1.5f;
        game_overlay_future_point_radius = 5.0f;
        game_overlay_future_alpha_falloff = 1.0f;

        game_overlay_icon_enabled = false;
        game_overlay_icon_path = "icon.png";
        game_overlay_icon_width = 64;
        game_overlay_icon_height = 64;
        game_overlay_icon_offset_x = 0.0f;
        game_overlay_icon_offset_y = 0.0f;
        game_overlay_icon_anchor = "center";

        show_recoil_indicator = false;

        // Custom classes
        class_player = { 0 };
        class_bot = { 1 };
        class_weapon = { 2 };
        class_outline = { 3 };
        class_dead_body = { 4 };
        class_hideout_target_human = { 5 };
        class_hideout_target_balls = { 6 };
        class_head = { 7 };
        class_smoke = { 8 };
        class_fire = { 9 };
        class_third_person = { 10 };

        // Debug
        show_window = true;
        show_fps = false;
        screenshot_button = splitString("None");
        screenshot_delay = 500;
        verbose = false;

        // Game profiles
        game_profiles.clear();
        GameProfile uni;
        uni.name = "UNIFIED";
        uni.sens = 1.0;
        uni.yaw = 0.022;
        uni.pitch = uni.yaw;
        uni.fovScaled = false;
        uni.baseFOV = 0.0;
        game_profiles[uni.name] = uni;
        active_game = uni.name;

        saveConfig(filename);
        return true;
    }

    CSimpleIniA ini;
    ini.SetUnicode();
    SI_Error rc = ini.LoadFile(filename.c_str());
    if (rc < 0)
    {
        std::cerr << "[Config] Error parsing INI file: " << filename << std::endl;
        return false;
    }

    auto get_string = [&](const char* key, const std::string& defval)
    {
        const char* val = ini.GetValue("", key, defval.c_str());
        return val ? std::string(val) : defval;
    };

    auto get_bool = [&](const char* key, bool defval)
        {
            return ini.GetBoolValue("", key, defval);
        };

    auto get_long = [&](const char* key, long defval)
        {
            return (int)ini.GetLongValue("", key, defval);
        };

    auto get_double = [&](const char* key, double defval)
        {
            return ini.GetDoubleValue("", key, defval);
        };

    game_profiles.clear();

    CSimpleIniA::TNamesDepend keys;
    ini.GetAllKeys("Games", keys);

    for (const auto& k : keys)
    {
        std::string name = k.pItem;
        std::string val = ini.GetValue("Games", k.pItem, "");
        auto parts = splitString(val, ',');

        try
        {
            if (parts.size() < 2)
                throw std::runtime_error("not enough values");

            GameProfile gp;
            gp.name = name;
            gp.sens = std::stod(parts[0]);
            gp.yaw = std::stod(parts[1]);
            gp.pitch = parts.size() > 2 ? std::stod(parts[2]) : gp.yaw;
            gp.fovScaled = parts.size() > 3 && (parts[3] == "true" || parts[3] == "1");
            gp.baseFOV = parts.size() > 4 ? std::stod(parts[4]) : 0.0;

            game_profiles[name] = gp;
        }
        catch (const std::exception& e)
        {
            std::cerr << "[Config] Failed to parse profile: " << name
                << " = " << val << " (" << e.what() << ")" << std::endl;
        }
    }

    if (!game_profiles.count("UNIFIED"))
    {
        GameProfile uni;
        uni.name = "UNIFIED";
        uni.sens = 1.0;
        uni.yaw = 0.022;
        uni.pitch = uni.yaw;
        uni.fovScaled = false;
        uni.baseFOV = 0.0;
        game_profiles[uni.name] = uni;
    }

    active_game = get_string("active_game", active_game);
    if (!game_profiles.count(active_game) && !game_profiles.empty())
        active_game = game_profiles.begin()->first;

    // Capture
    capture_method = get_string("capture_method", "duplication_api");
    capture_target = get_string("capture_target", "monitor");
    capture_window_title = get_string("capture_window_title", "");
    gstreamer_pipeline = get_string("gstreamer_pipeline", "");
    detection_resolution = get_long("detection_resolution", 320);
    if (detection_resolution != 160 && detection_resolution != 320 && detection_resolution != 640)
        detection_resolution = 320;

    capture_fps = get_long("capture_fps", 60);
    monitor_idx = get_long("monitor_idx", 0);
    circle_mask = get_bool("circle_mask", true);
    capture_borders = get_bool("capture_borders", true);
    capture_cursor = get_bool("capture_cursor", true);
    virtual_camera_name = get_string("virtual_camera_name", "None");
    virtual_camera_width = get_long("virtual_camera_width", 1920);
    virtual_camera_heigth = get_long("virtual_camera_heigth", 1080);

    // Target
    disable_headshot = get_bool("disable_headshot", false);
    body_y_offset = (float)get_double("body_y_offset", 0.15);
    head_y_offset = (float)get_double("head_y_offset", 0.05);
    auto_aim = get_bool("auto_aim", false);

    // Mouse
    fovX = get_long("fovX", 106);
    fovY = get_long("fovY", 74);
    minSpeedMultiplier = (float)get_double("minSpeedMultiplier", 0.1);
    maxSpeedMultiplier = (float)get_double("maxSpeedMultiplier", 0.1);

    predictionInterval = (float)get_double("predictionInterval", 0.01);
    prediction_futurePositions = get_long("prediction_futurePositions", 20);
    draw_futurePositions = get_bool("draw_futurePositions", true);
    
    snapRadius = (float)get_double("snapRadius", 1.5);
    nearRadius = (float)get_double("nearRadius", 25.0);
    speedCurveExponent = (float)get_double("speedCurveExponent", 3.0);
    snapBoostFactor = (float)get_double("snapBoostFactor", 1.15);

    easynorecoil = get_bool("easynorecoil", false);
    easynorecoilstrength = (float)get_double("easynorecoilstrength", 0.0);
    easynorecoil_offsetY = (float)get_double("easynorecoil_offsetY", 0.0);
    easynorecoil_increaseSpeed = (float)get_double("easynorecoil_increaseSpeed", 1.0);
    easynorecoil_returnSpeed = (float)get_double("easynorecoil_returnSpeed", 1.0);
    input_method = get_string("input_method", "WIN32");

    // Wind mouse
    wind_mouse_enabled = get_bool("wind_mouse_enabled", false);
    wind_G = (float)get_double("wind_G", 18.0f);
    wind_W = (float)get_double("wind_W", 15.0f);
    wind_M = (float)get_double("wind_M", 10.0f);
    wind_D = (float)get_double("wind_D", 8.0f);
    starts_after_mouse_move = get_bool("starts_after_mouse_move", false);
    move_timeout = (int)get_double("move_timeout", 10);
    min_mouse_move_length = (int)get_double("min_mouse_move_length", 1);
    aim_timeout = (int)get_double("aim_timeout", 10);
    max_aim_distance = (int)get_double("max_aim_distance", 1);

    // Arduino
    arduino_baudrate = get_long("arduino_baudrate", 115200);
    arduino_port = get_string("arduino_port", "COM0");
    arduino_16_bit_mouse = get_bool("arduino_16_bit_mouse", false);
    arduino_enable_keys = get_bool("arduino_enable_keys", false);

    // MIDI
    midi_device_name = get_string("midi_device_name", "");

    // kmbox_B
    kmbox_b_baudrate = get_long("kmbox_baudrate", 115200);
    kmbox_b_port = get_string("kmbox_port", "COM0");

    // kmbox_net
    kmbox_net_ip = get_string("kmbox_net_ip", "10.42.42.42");
    kmbox_net_port = get_string("kmbox_net_port", "1984");
    kmbox_net_uuid = get_string("kmbox_net_uuid", "DEADC0DE");

    // makcu
    makcu_baudrate = get_long("makcu_baudrate", 115200);
    makcu_port = get_string("makcu_port", "COM0");

    // Mouse shooting
    auto_shoot = get_bool("auto_shoot", false);
    bScope_multiplier = (float)get_double("bScope_multiplier", 1.2);

    // AI
#ifdef USE_CUDA
    backend = get_string("backend", "TRT");
#else
    backend = get_string("backend", "DML");
#endif

    dml_device_id = get_long("dml_device_id", 0);

#ifdef USE_CUDA
    ai_model = get_string("ai_model", "sunxds_0.5.6.engine");
#else
    ai_model = get_string("ai_model", "sunxds_0.5.6.onnx");
#endif
    confidence_threshold = (float)get_double("confidence_threshold", 0.15);
    nms_threshold = (float)get_double("nms_threshold", 0.50);
    max_detections = get_long("max_detections", 20);
#ifdef USE_CUDA
    export_enable_fp8 = get_bool("export_enable_fp8", true);
    export_enable_fp16 = get_bool("export_enable_fp16", true);
#endif
    fixed_input_size = get_bool("fixed_input_size", false);

    // CUDA
#ifdef USE_CUDA
    use_cuda_graph = get_bool("use_cuda_graph", false);
    use_pinned_memory = get_bool("use_pinned_memory", true);
    gpuMemoryReserveMB = get_long("gpuMemoryReserveMB", 2048);
    enableGpuExclusiveMode = get_bool("enableGpuExclusiveMode", true);
#endif

    // System
    cpuCoreReserveCount = get_long("cpuCoreReserveCount", 4);
    systemMemoryReserveMB = get_long("systemMemoryReserveMB", 2048);

    // Buttons
    button_targeting = splitString(get_string("button_targeting", "RightMouseButton"));
    button_shoot = splitString(get_string("button_shoot", "LeftMouseButton"));
    button_zoom = splitString(get_string("button_zoom", "RightMouseButton"));
    button_exit = splitString(get_string("button_exit", "F2"));
    button_pause = splitString(get_string("button_pause", "F3"));
    button_reload_config = splitString(get_string("button_reload_config", "F4"));
    button_open_overlay = splitString(get_string("button_open_overlay", "Home"));
    enable_arrows_settings = get_bool("enable_arrows_settings", false);

    std::string binds_line = get_string("binds", "");
    bind_keys.clear(); bind_filenames.clear();
    if (!binds_line.empty()) {
        auto pairs = splitString(binds_line, ',');
        for (auto& p : pairs) {
            auto colon = p.find(':');
            if (colon != std::string::npos) {
                std::string k = p.substr(0, colon);
                std::string f = p.substr(colon + 1);
                // trim
                while (!k.empty() && isspace((unsigned char)k.front())) k.erase(k.begin());
                while (!k.empty() && isspace((unsigned char)k.back())) k.pop_back();
                while (!f.empty() && isspace((unsigned char)f.front())) f.erase(f.begin());
                while (!f.empty() && isspace((unsigned char)f.back())) f.pop_back();
                bind_keys.push_back(k);
                bind_filenames.push_back(f);
            }
        }
    }

    // Overlay
    overlay_opacity = get_long("overlay_opacity", 225);
    overlay_ui_scale = (float)get_double("overlay_ui_scale", 1.0);

    // Depth
    depth_model_path = get_string("depth_model_path", "depth_anything_v2.engine");
    depth_fps = get_long("depth_fps", 100);
    if (depth_fps < 0) depth_fps = 0;

    game_overlay_enabled = get_bool("game_overlay_enabled", false);
    game_overlay_max_fps = get_long("game_overlay_max_fps", 0);
    game_overlay_draw_boxes = get_bool("game_overlay_draw_boxes", true);
    game_overlay_draw_future = get_bool("game_overlay_draw_future", true);
    game_overlay_draw_frame = get_bool("game_overlay_draw_frame", true);
    game_overlay_show_target_correction = get_bool("game_overlay_show_target_correction", true);
    game_overlay_box_a = get_long("game_overlay_box_a", 255);
    game_overlay_box_r = get_long("game_overlay_box_r", 0);
    game_overlay_box_g = get_long("game_overlay_box_g", 255);
    game_overlay_box_b = get_long("game_overlay_box_b", 0);
    game_overlay_frame_a = get_long("game_overlay_frame_a", 180);
    game_overlay_frame_r = get_long("game_overlay_frame_r", 255);
    game_overlay_frame_g = get_long("game_overlay_frame_g", 255);
    game_overlay_frame_b = get_long("game_overlay_frame_b", 255);
    game_overlay_box_thickness = (float)get_double("game_overlay_box_thickness", 2.0);
    game_overlay_frame_thickness = (float)get_double("game_overlay_frame_thickness", 1.5);
    game_overlay_future_point_radius = (float)get_double("game_overlay_future_point_radius", 5.0);
    game_overlay_future_alpha_falloff = (float)get_double("game_overlay_future_alpha_falloff", 1.0);
    clampGameOverlayColor();

    game_overlay_icon_enabled = get_bool("game_overlay_icon_enabled", false);
    game_overlay_icon_path = get_string("game_overlay_icon_path", "icon.png");
    game_overlay_icon_width = get_long("game_overlay_icon_width", 64);
    game_overlay_icon_height = get_long("game_overlay_icon_height", 64);
    game_overlay_icon_offset_x = (float)get_double("game_overlay_icon_offset_x", 0.0f);
    game_overlay_icon_offset_y = (float)get_double("game_overlay_icon_offset_y", 0.0f);
    game_overlay_icon_anchor = get_string("game_overlay_icon_anchor", "center");

    show_recoil_indicator = get_bool("show_recoil_indicator", false);

    // Custom Classes
    class_player = parseClassList(get_string("class_player", "0"));
    class_bot = parseClassList(get_string("class_bot", "1"));
    class_weapon = parseClassList(get_string("class_weapon", "2"));
    class_outline = parseClassList(get_string("class_outline", "3"));
    class_dead_body = parseClassList(get_string("class_dead_body", "4"));
    class_hideout_target_human = parseClassList(get_string("class_hideout_target_human", "5"));
    class_hideout_target_balls = parseClassList(get_string("class_hideout_target_balls", "6"));
    class_head = parseClassList(get_string("class_head", "7"));
    class_smoke = parseClassList(get_string("class_smoke", "8"));
    class_fire = parseClassList(get_string("class_fire", "9"));
    class_third_person = parseClassList(get_string("class_third_person", "10"));

    // Debug window
    show_window = get_bool("show_window", true);
    screenshot_button = splitString(get_string("screenshot_button", "None"));
    screenshot_delay = get_long("screenshot_delay", 500);
    verbose = get_bool("verbose", false);

    return true;
}

bool Config::saveConfig(const std::string& filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error opening config for writing: " << filename << std::endl;
        return false;
    }

    file << "# An explanation of the options can be found at:\n";
    file << "# https://github.com/SunOner/sunone_aimbot_docs/blob/main/config/config_cpp.md\n\n";

    // Capture
    file << "# Capture\n"
        << "capture_method = " << capture_method << "\n"
        << "capture_target = " << capture_target << "\n"
        << "capture_window_title = " << capture_window_title << "\n"
        << "gstreamer_pipeline = " << gstreamer_pipeline << "\n"
        << "detection_resolution = " << detection_resolution << "\n"
        << "capture_fps = " << capture_fps << "\n"
        << "monitor_idx = " << monitor_idx << "\n"
        << "circle_mask = " << (circle_mask ? "true" : "false") << "\n"
        << "capture_borders = " << (capture_borders ? "true" : "false") << "\n"
        << "capture_cursor = " << (capture_cursor ? "true" : "false") << "\n"
        << "virtual_camera_name = " << virtual_camera_name << "\n"
        << "virtual_camera_width = " << virtual_camera_width << "\n"
        << "virtual_camera_heigth = " << virtual_camera_heigth << "\n\n";

    // Target
    file << "# Target\n"
        << "disable_headshot = " << (disable_headshot ? "true" : "false") << "\n"
        << std::fixed << std::setprecision(2)
        << "body_y_offset = " << body_y_offset << "\n"
        << "head_y_offset = " << head_y_offset << "\n"
        << "auto_aim = " << (auto_aim ? "true" : "false") << "\n\n";

    // Mouse
    file << "# Mouse move\n"
        << "fovX = " << fovX << "\n"
        << "fovY = " << fovY << "\n"
        << "minSpeedMultiplier = " << minSpeedMultiplier << "\n"
        << "maxSpeedMultiplier = " << maxSpeedMultiplier << "\n"

        << std::fixed << std::setprecision(2)
        << "predictionInterval = " << predictionInterval << "\n"
        << "prediction_futurePositions = " << prediction_futurePositions << "\n"
        << "draw_futurePositions = " << (draw_futurePositions ? "true" : "false") << "\n"

        << "snapRadius = " << snapRadius << "\n"
        << "nearRadius = " << nearRadius << "\n"
        << "speedCurveExponent = " << speedCurveExponent << "\n"
        << std::fixed << std::setprecision(2)
        << "snapBoostFactor = " << snapBoostFactor << "\n"

        << "easynorecoil = " << (easynorecoil ? "true" : "false") << "\n"
        << std::fixed << std::setprecision(1)
        << "easynorecoilstrength = " << easynorecoilstrength << "\n"

        << std::fixed << std::setprecision(2)
        << "easynorecoil_offsetY = " << easynorecoil_offsetY << "\n"
        << "easynorecoil_increaseSpeed = " << easynorecoil_increaseSpeed << "\n"
        << "easynorecoil_returnSpeed = " << easynorecoil_returnSpeed << "\n"

        << "# WIN32, GHUB, ARDUINO, MIDI, KMBOX_B, KMBOX_NET, MACKU\n"
        << "input_method = " << input_method << "\n\n";

    // Wind mouse
    file << "# Wind mouse\n"
        << "wind_mouse_enabled = " << (wind_mouse_enabled ? "true" : "false") << "\n"
        << "wind_G = " << wind_G << "\n"
        << "wind_W = " << wind_W << "\n"
        << "wind_M = " << wind_M << "\n"
        << "wind_D = " << wind_D << "\n"
        << "starts_after_mouse_move = " << starts_after_mouse_move << "\n"
        << "move_timeout = " << move_timeout << "\n"
        << "min_mouse_move_length = " << min_mouse_move_length << "\n"
        << "aim_timeout = " << aim_timeout << "\n"
        << "max_aim_distance = " << max_aim_distance << "\n\n";

    // Arduino
    file << "# Arduino\n"
        << "arduino_baudrate = " << arduino_baudrate << "\n"
        << "arduino_port = " << arduino_port << "\n"
        << "arduino_16_bit_mouse = " << (arduino_16_bit_mouse ? "true" : "false") << "\n"
        << "arduino_enable_keys = " << (arduino_enable_keys ? "true" : "false") << "\n\n";

    // MIDI
    file << "# MIDI\n"
        << "midi_device_name = " << midi_device_name << "\n\n";

    // kmbox_B
    file << "# Kmbox_B\n"
        << "kmbox_baudrate = " << kmbox_b_baudrate << "\n"
        << "kmbox_port = " << kmbox_b_port << "\n\n";

    // kmbox_net
    file << "# Kmbox_net\n"
        << "kmbox_net_ip = " << kmbox_net_ip << "\n"
        << "kmbox_net_port = " << kmbox_net_port << "\n"
        << "kmbox_net_uuid = " << kmbox_net_uuid << "\n\n";

    // makcu
    file << "# Makcu\n"
        << "makcu_baudrate = " << makcu_baudrate << "\n"
		<< "makcu_port = " << makcu_port << "\n\n";

    // Mouse shooting
    file << "# Mouse shooting\n"
        << "auto_shoot = " << (auto_shoot ? "true" : "false") << "\n"
        << std::fixed << std::setprecision(1)
        << "bScope_multiplier = " << bScope_multiplier << "\n\n";

    // AI
    file << "# AI\n"
        << "backend = " << backend << "\n"
        << "dml_device_id = " << dml_device_id << "\n"
        << "ai_model = " << ai_model << "\n"
        << std::fixed << std::setprecision(2)
        << "confidence_threshold = " << confidence_threshold << "\n"
        << "nms_threshold = " << nms_threshold << "\n"
        << std::setprecision(0)
        << "max_detections = " << max_detections << "\n"
#ifdef USE_CUDA
        << "export_enable_fp8 = " << (export_enable_fp8 ? "true" : "false") << "\n"
        << "export_enable_fp16 = " << (export_enable_fp16 ? "true" : "false") << "\n"
#endif
        << "fixed_input_size = " << (fixed_input_size ? "true" : "false") << "\n";

    // CUDA
#ifdef USE_CUDA
    file << "\n# CUDA\n"
        << "use_cuda_graph = " << (use_cuda_graph ? "true" : "false") << "\n"
        << "use_pinned_memory = " << (use_pinned_memory ? "true" : "false") << "\n"
        << "gpuMemoryReserveMB = " << gpuMemoryReserveMB << "\n"
        << "enableGpuExclusiveMode = " << (enableGpuExclusiveMode ? "true" : "false") << "\n\n";
#endif

	// System
    file << "# System\n"
        << "cpuCoreReserveCount = " << cpuCoreReserveCount << "\n"
        << "systemMemoryReserveMB = " << systemMemoryReserveMB << "\n\n";

    // Buttons
    file << "# Buttons\n"
        << "button_targeting = " << joinStrings(button_targeting) << "\n"
        << "button_shoot = " << joinStrings(button_shoot) << "\n"
        << "button_zoom = " << joinStrings(button_zoom) << "\n"
        << "button_exit = " << joinStrings(button_exit) << "\n"
        << "button_pause = " << joinStrings(button_pause) << "\n"
        << "button_reload_config = " << joinStrings(button_reload_config) << "\n"
        << "button_open_overlay = " << joinStrings(button_open_overlay) << "\n"
        << "binds = " << joinKeyFilePairs(bind_keys, bind_filenames) << "\n"
        << "enable_arrows_settings = " << (enable_arrows_settings ? "true" : "false") << "\n\n";

    // Overlay
    file << "# Overlay\n"
        << "overlay_opacity = " << overlay_opacity << "\n"
        << std::fixed << std::setprecision(2)
        << "overlay_ui_scale = " << overlay_ui_scale << "\n\n";

    file << "# Depth\n"
        << "depth_model_path = " << depth_model_path << "\n"
        << "depth_fps = " << depth_fps << "\n\n";

    file << "# Game Overlay\n"
        << "game_overlay_enabled = " << (game_overlay_enabled ? "true" : "false") << "\n"
        << "game_overlay_max_fps = " << game_overlay_max_fps << "\n"
        << "game_overlay_draw_boxes = " << (game_overlay_draw_boxes ? "true" : "false") << "\n"
        << "game_overlay_draw_future = " << (game_overlay_draw_future ? "true" : "false") << "\n"
        << "game_overlay_draw_frame = " << (game_overlay_draw_frame ? "true" : "false") << "\n"
        << "game_overlay_show_target_correction = " << (game_overlay_show_target_correction ? "true" : "false") << "\n"
        << "game_overlay_box_a = " << game_overlay_box_a << "\n"
        << "game_overlay_box_r = " << game_overlay_box_r << "\n"
        << "game_overlay_box_g = " << game_overlay_box_g << "\n"
        << "game_overlay_box_b = " << game_overlay_box_b << "\n"
        << "game_overlay_frame_a = " << game_overlay_frame_a << "\n"
        << "game_overlay_frame_r = " << game_overlay_frame_r << "\n"
        << "game_overlay_frame_g = " << game_overlay_frame_g << "\n"
        << "game_overlay_frame_b = " << game_overlay_frame_b << "\n"
        << std::fixed << std::setprecision(2)
        << "game_overlay_box_thickness = " << game_overlay_box_thickness << "\n"
        << "game_overlay_frame_thickness = " << game_overlay_frame_thickness << "\n"
        << "game_overlay_future_point_radius = " << game_overlay_future_point_radius << "\n"
        << "game_overlay_future_alpha_falloff = " << game_overlay_future_alpha_falloff << "\n\n";

    file << "game_overlay_icon_enabled = " << (game_overlay_icon_enabled ? "true" : "false") << "\n"
        << "game_overlay_icon_path = " << game_overlay_icon_path << "\n"
        << "game_overlay_icon_width = " << game_overlay_icon_width << "\n"
        << "game_overlay_icon_height = " << game_overlay_icon_height << "\n"
        << std::fixed << std::setprecision(2)
        << "game_overlay_icon_offset_x = " << game_overlay_icon_offset_x << "\n"
        << std::fixed << std::setprecision(2)
        << "game_overlay_icon_offset_y = " << game_overlay_icon_offset_y << "\n"
        << "game_overlay_icon_anchor = " << game_overlay_icon_anchor << "\n\n";

    file << "show_recoil_indicator = " << (show_recoil_indicator ? "true" : "false") << "\n\n";

    auto joinInts = [](const std::vector<int>& vec) -> std::string {
        std::ostringstream oss;
        for (size_t i = 0; i < vec.size(); ++i)
        {
            if (i > 0) oss << ", ";
            oss << vec[i];
        }
        return oss.str();
        };

    // Custom Classes
    file << "# Custom Classes\n"
        << "class_player = " << joinInts(class_player) << "\n"
        << "class_bot = " << joinInts(class_bot) << "\n"
        << "class_weapon = " << joinInts(class_weapon) << "\n"
        << "class_outline = " << joinInts(class_outline) << "\n"
        << "class_dead_body = " << joinInts(class_dead_body) << "\n"
        << "class_hideout_target_human = " << joinInts(class_hideout_target_human) << "\n"
        << "class_hideout_target_balls = " << joinInts(class_hideout_target_balls) << "\n"
        << "class_head = " << joinInts(class_head) << "\n"
        << "class_smoke = " << joinInts(class_smoke) << "\n"
        << "class_fire = " << joinInts(class_fire) << "\n"
        << "class_third_person = " << joinInts(class_third_person) << "\n\n";

    // Debug
    file << "# Debug\n"
        << "show_window = " << (show_window ? "true" : "false") << "\n"
        << "show_fps = " << (show_fps ? "true" : "false") << "\n"
        << "screenshot_button = " << joinStrings(screenshot_button) << "\n"
        << "screenshot_delay = " << screenshot_delay << "\n"
        << "verbose = " << (verbose ? "true" : "false") << "\n\n";

    // Active game
    file << "# Active game profile\n";
    file << "active_game = " << active_game << "\n\n";
    file << "[Games]\n";
    for (auto& kv : game_profiles)
    {
        auto & gp = kv.second;
        file << gp.name << " = "
             << gp.sens << "," << gp.yaw;
        file << "," << gp.pitch;
        if (gp.fovScaled)
            file << ",true," << gp.baseFOV;
        file << "\n";
    }

    file.close();
    return true;
}

// возвращает относительные пути к файлам *.ini в папке binds
std::vector<std::string> Config::listBindFiles(const std::string& folder) const
{
    std::vector<std::string> files;
    try {
        if (!std::filesystem::exists(folder)) {
            std::filesystem::create_directory(folder);
            return files;
        }
        for (auto& p : std::filesystem::directory_iterator(folder)) {
            if (!p.is_regular_file()) continue;
            auto ext = p.path().extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if (ext == ".ini" || ext == ".cfg" || ext == ".txt") {
                files.push_back(p.path().string());
            }
        }
    }
    catch (...) {}
    std::sort(files.begin(), files.end());
    return files;
}

bool Config::applyPartialConfigFile(const std::string& filename)
{
    if (!std::filesystem::exists(filename)) return false;

    CSimpleIniA ini;
    ini.SetUnicode();
    SI_Error rc = ini.LoadFile(filename.c_str());
    if (rc < 0) return false;

    auto tryGetString = [&](const char* k, std::string& out) -> bool {
        const char* v = ini.GetValue("", k, nullptr);
        if (v) { out = v; return true; }
        return false;
        };
    auto tryGetBool = [&](const char* k, bool& out) -> bool {
        const char* v = ini.GetValue("", k, nullptr);
        if (v) { out = (std::string(v) == "true" || std::string(v) == "1"); return true; }
        return false;
        };
    auto tryGetLong = [&](const char* k, long& out) -> bool {
        const char* v = ini.GetValue("", k, nullptr);
        if (v) { out = ini.GetLongValue("", k, out); return true; }
        return false;
        };
    auto tryGetDouble = [&](const char* k, double& out) -> bool {
        const char* v = ini.GetValue("", k, nullptr);
        if (v) { out = ini.GetDoubleValue("", k, out); return true; }
        return false;
        };
    auto get_string = [&](const char* key, const std::string& defval)
        {
            const char* val = ini.GetValue("", key, defval.c_str());
            return val ? std::string(val) : defval;
        };

    // --- Примеры параметров (добавляй/удаляй по необходимости) ---
    double tmpd;
    long tmpl;
    bool tmpb;
    std::string tmps;

    //Easy No Recoil
    if (tryGetDouble("easynorecoil_offsetY", tmpd)) easynorecoil_offsetY = (float)tmpd;
    if (tryGetDouble("easynorecoil_increaseSpeed", tmpd)) easynorecoil_increaseSpeed = (float)tmpd;
    if (tryGetDouble("easynorecoil_returnSpeed", tmpd)) easynorecoil_returnSpeed = (float)tmpd;
    if (tryGetDouble("easynorecoilstrength", tmpd)) easynorecoilstrength = (float)tmpd;
    if (tryGetBool("easynorecoil", tmpb)) easynorecoil = tmpb;

    //Target correction
    if (tryGetLong("fovX", tmpl)) fovX = (int)tmpl;
    if (tryGetLong("fovY", tmpl)) fovY = (int)tmpl;
    if (tryGetDouble("minSpeedMultiplier", tmpd)) minSpeedMultiplier = (float)tmpd;
    if (tryGetDouble("maxSpeedMultiplier", tmpd)) maxSpeedMultiplier = (float)tmpd;
    if (tryGetDouble("predictionInterval", tmpd)) predictionInterval = (float)tmpd;
    if (tryGetLong("prediction_futurePositions", tmpl)) prediction_futurePositions = (int)tmpl;
    if (tryGetBool("draw_futurePositions", tmpb)) draw_futurePositions = tmpb;

    //Wnd mouse
    if (tryGetBool("wind_mouse_enabled", tmpb)) wind_mouse_enabled = tmpb;
    if (tryGetDouble("wind_G", tmpd)) wind_G = (float)tmpd;
    if (tryGetDouble("wind_W", tmpd)) wind_W = (float)tmpd;
    if (tryGetDouble("wind_M", tmpd)) wind_M = (float)tmpd;
    if (tryGetDouble("wind_D", tmpd)) wind_D = (float)tmpd;

    //Target
    if (tryGetBool("disable_headshot", tmpb)) disable_headshot = tmpb;
    if (tryGetDouble("body_y_offset", tmpd)) body_y_offset = (float)tmpd;
    if (tryGetDouble("head_y_offset ", tmpd)) head_y_offset = (float)tmpd;
    if (tryGetBool("auto_aim ", tmpb)) auto_aim = tmpb;

    //AI
    if (tryGetString("ai_model", tmps)) ai_model = tmps;
    if (tryGetString("postprocess", tmps)) postprocess = tmps;
    if (tryGetDouble("confidence_threshold", tmpd)) confidence_threshold = (float)tmpd;
    if (tryGetDouble("nms_threshold", tmpd)) nms_threshold = (float)tmpd;

    // Buttons override
    if (tryGetString("button_targeting", tmps)) button_targeting = splitString(tmps);
    if (tryGetString("button_shoot", tmps)) button_shoot = splitString(tmps);
    if (tryGetString("button_zoom", tmps)) button_zoom = splitString(tmps);

    // Custom classes
    if (tryGetString("class_player", tmps)) {
        auto parts = splitString(tmps, ',');
        if (!parts.empty()) {
            class_player = parseClassList(get_string("class_player", "0"));
        }
    }
    if (tryGetString("class_head", tmps)) {
        auto parts = splitString(tmps, ',');
        if (!parts.empty())
        {
            class_head = parseClassList(get_string("class_head", "0"));
        }
    }

    return true;
}

const Config::GameProfile& Config::currentProfile() const
{
    auto it = game_profiles.find(active_game);
    if (it != game_profiles.end()) return it->second;
    throw std::runtime_error("Active game profile not found: " + active_game);
}

std::pair<double, double> Config::degToCounts(double degX, double degY, double fovNow) const
{
    const auto& gp = currentProfile();
    double scale = (gp.fovScaled && gp.baseFOV > 1.0) ? (fovNow / gp.baseFOV) : 1.0;

    if (gp.sens == 0.0 || gp.yaw == 0.0 || gp.pitch == 0.0)
        return { 0.0, 0.0 };

    double cx = degX / (gp.sens * gp.yaw * scale);
    double cy = degY / (gp.sens * gp.pitch * scale);
    return { cx, cy };
}
