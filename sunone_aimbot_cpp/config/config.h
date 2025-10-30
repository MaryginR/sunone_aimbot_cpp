#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <unordered_map>
#include <utility>

class Config
{
public:
    // Capture
    std::string capture_method; // "duplication_api", "winrt", "virtual_camera"
    int detection_resolution;
    int capture_fps;
    int monitor_idx;
    bool circle_mask;
    bool capture_borders;
    bool capture_cursor;
    std::string virtual_camera_name;
    int virtual_camera_width;
    int virtual_camera_heigth;

    // Target
    bool disable_headshot;
    float body_y_offset;
    float head_y_offset;
    bool ignore_third_person;
    bool shooting_range_targets;
    bool auto_aim;

    // Mouse
    int fovX;
    int fovY;
    float minSpeedMultiplier;
    float maxSpeedMultiplier;

    float predictionInterval;
    int prediction_futurePositions;
    bool draw_futurePositions;

    float snapRadius;
    float nearRadius;
    float speedCurveExponent;
    float snapBoostFactor;

    bool easynorecoil;
    float easynorecoilstrength;
    float easynorecoil_offsetY;        // ������������ �������� �� ��� Y
    float easynorecoil_increaseSpeed;  // �������� ���������� ��������
    float easynorecoil_returnSpeed;    // �������� ��������
    std::string input_method; // "WIN32", "GHUB", "ARDUINO", "KMBOX_B", "KMBOX_NET"

    // Wind mouse
    bool wind_mouse_enabled;
    float wind_G;
    float wind_W;
    float wind_M;
    float wind_D;

    // Arduino
    int arduino_baudrate;
    std::string arduino_port;
    bool arduino_16_bit_mouse;
    bool arduino_enable_keys;

    // kmbox_b
    int kmbox_b_baudrate;
    std::string kmbox_b_port;

    // kmbox_net
    std::string kmbox_net_ip;
    std::string kmbox_net_port;
    std::string kmbox_net_uuid;

    // Mouse shooting
    bool auto_shoot;
    float bScope_multiplier;

    // AI
    std::string backend;
    int dml_device_id;
    std::string ai_model;
    float confidence_threshold;
    float nms_threshold;
    int max_detections;
    std::string postprocess;
    int batch_size;
#ifdef USE_CUDA
    bool export_enable_fp8;
    bool export_enable_fp16;
#endif
    bool fixed_input_size;

    // CUDA
#ifdef USE_CUDA
    bool use_cuda_graph;
    bool use_pinned_memory;
#endif
    // Buttons
    std::vector<std::string> button_targeting;
    std::vector<std::string> button_shoot;
    std::vector<std::string> button_zoom;
    std::vector<std::string> button_exit;
    std::vector<std::string> button_pause;
    std::vector<std::string> button_reload_config;
    std::vector<std::string> button_open_overlay;
    bool enable_arrows_settings;

    std::vector<std::string> bind_keys;       // e.g. "F1", "F2", "Mouse4"
    std::vector<std::string> bind_filenames;  // e.g. "binds/sniper.ini", "binds/recoil.ini"

    // Overlay
    int overlay_opacity;
    bool overlay_snow_theme;
    float overlay_ui_scale;

    // Game Overlay
    bool game_overlay_enabled;
    int game_overlay_max_fps;
    bool game_overlay_draw_boxes;
    bool game_overlay_draw_future;
    int game_overlay_box_a;
    int game_overlay_box_r;
    int game_overlay_box_g;
    int game_overlay_box_b;
    float game_overlay_box_thickness;
    float game_overlay_future_point_radius;
    float game_overlay_future_alpha_falloff;

    bool game_overlay_icon_enabled;
    std::string game_overlay_icon_path;
    int game_overlay_icon_width;
    int game_overlay_icon_height;
    float game_overlay_icon_offset_x;
    float game_overlay_icon_offset_y;
    std::string game_overlay_icon_anchor; // "center", "top", "bottom", "head"

    void clampGameOverlayColor()
    {
        auto clamp255 = [](int& v) { if (v < 0) v = 0; if (v > 255) v = 255; };
        clamp255(game_overlay_box_a);
        clamp255(game_overlay_box_r);
        clamp255(game_overlay_box_g);
        clamp255(game_overlay_box_b);
    }

    // Custom Classes
    std::vector<int> class_player;               //0
    std::vector<int> class_bot;                  //1
    std::vector<int> class_weapon;               //2
    std::vector<int> class_outline;              //3
    std::vector<int> class_dead_body;            //4
    std::vector<int> class_hideout_target_human; //5
    std::vector<int> class_hideout_target_balls; //6
    std::vector<int> class_head;                 //7
    std::vector<int> class_smoke;                //8
    std::vector<int> class_fire;                 //9
    std::vector<int> class_third_person;         //10

    // Debug
    bool show_window;
    bool show_fps;
    std::vector<std::string> screenshot_button;
    int screenshot_delay;
    bool verbose;

    struct GameProfile
    {
        std::string name;
        double sens;
        double yaw;
        double pitch;
        bool fovScaled;
        double baseFOV;
    };

    std::unordered_map<std::string, GameProfile> game_profiles;
    std::string                                  active_game;

    const GameProfile & currentProfile() const;
    std::pair<double, double> degToCounts(double degX, double degY, double fovNow) const;

    // helper ��� ��������� ������ ������ ������ � �����
    std::vector<std::string> listBindFiles(const std::string& folder = "binds") const;

    // new: apply only fields from ini file into current config (partial update)
    bool applyPartialConfigFile(const std::string& filename);

    bool loadConfig(const std::string& filename = "config.ini");
    bool saveConfig(const std::string& filename = "config.ini");

    std::string joinStrings(const std::vector<std::string>& vec, const std::string& delimiter = ",");
private:
    std::vector<std::string> splitString(const std::string& str, char delimiter = ',');
};

#endif // CONFIG_H