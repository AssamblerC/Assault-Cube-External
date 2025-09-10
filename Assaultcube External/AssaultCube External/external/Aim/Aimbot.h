#pragma once
#include <Windows.h>
#include <vector>
#include <chrono> 
#include "../Utility/ExternMath.h"
#include "../Memory/GetValues.h" 
#include "../Overlay/Menu.h"

class Aimbot {
public:
    Aimbot();
    void aimUpdate(const GetValues::MemoryValues& memValues, const Mathematics& math, const Menu::MenuOptions& menuOptions);
    void SetFov(float fov);

private:
    void PerformAim(const std::vector<float>& player_pos, const std::vector<float>& enemy_pos, float player_yaw, float player_pitch, const Mathematics& math, const Menu::MenuOptions& menuOptions);
    void HumanizedMoveMouse(float target_yaw, float target_pitch, float current_yaw, float current_pitch, const Menu::MenuOptions& menuOptions);

    double m_move_v_x, m_move_v_y; // Velocity
    double m_move_W_x, m_move_W_y; // Wind force

    int m_currentTargetIndex;
    std::chrono::steady_clock::time_point m_cooldownUntil;
    bool m_isSpeedReset;
    std::chrono::steady_clock::time_point m_speedResetUntil;

    int m_cooldownMs;
};