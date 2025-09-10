#include "Aimbot.h"
#include <cfloat> // For FLT_MAX
#include "../Memory/GetValues.h"

Aimbot::Aimbot() {
    m_currentTargetIndex = -1;
    m_cooldownUntil = {};
    m_cooldownMs = 500;  
    m_move_v_x = 0.0;
    m_move_v_y = 0.0;
    m_move_W_x = 0.0;
    m_move_W_y = 0.0;
    m_isSpeedReset = false;
    m_speedResetUntil = {};
}

void Aimbot::aimUpdate(const GetValues::MemoryValues& memoryValues, const Mathematics& math, const Menu::MenuOptions& menuValues) {

    if (!menuValues.AimbotEnabled) {
        m_move_v_x = 0;
        m_move_v_y = 0;
        return;
    }

    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 || GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
        if (std::chrono::steady_clock::now() < m_cooldownUntil) {
        }
        else {
            if (m_currentTargetIndex != -1 && memoryValues.enemyIsDead[m_currentTargetIndex] != 0) {
                m_cooldownUntil = std::chrono::steady_clock::now() + std::chrono::milliseconds(m_cooldownMs);
                m_currentTargetIndex = -1;
            }
            else {
                float closest_distance = FLT_MAX;
                int closest_enemy_index = -1;
                bool isTeamMode = IsTeamGameMode(memoryValues.gameMode);

                for (int i = 0; i < memoryValues.playercount; ++i) {

                    bool isTargetableByTeam = !isTeamMode || (memoryValues.enemyTeam[i] != memoryValues.playerTeam);
                    bool isTargetValid = memoryValues.enemyIsDead[i] == 0 && isTargetableByTeam;

                    if (menuValues.AimbotVisCheck) {
                        isTargetValid = isTargetValid && memoryValues.enemyIsVisible[i];
                    }
                    if (isTargetValid) {
                        vec3d_f enemy_screen_pos;
                        if (math.WorldToScreen(memoryValues.enemyposvec[i], enemy_screen_pos, const_cast<float*>(memoryValues.vMatrix), memoryValues.imguiSizeX, memoryValues.imguiSizeY)) {
                            vec3d_f crosshairPos = { memoryValues.imguiSizeX / 2, memoryValues.imguiSizeY / 2, 0.0f };
                            float crosshair_dist = math.GetDistance2D(crosshairPos, enemy_screen_pos);

                            if (crosshair_dist < menuValues.AimbotFov) { 
                                if (crosshair_dist < closest_distance) {
                                    closest_distance = crosshair_dist;
                                    closest_enemy_index = i;
                                }
                            }
                        }
                    }
                }

                if (closest_enemy_index != -1) {
                    PerformAim(memoryValues.playerposvec, memoryValues.enemyposvec[closest_enemy_index], memoryValues.playerYawAngle, memoryValues.playerPitchAngle, math, menuValues);
                    m_currentTargetIndex = closest_enemy_index;
                }
                else {
                    m_currentTargetIndex = -1;
                }
            }
        }
    }
    else {
        m_currentTargetIndex = -1;
    }
}

void Aimbot::PerformAim(const std::vector<float>& player_pos, const std::vector<float>& enemy_pos, float player_yaw, float player_pitch, const Mathematics& math, const Menu::MenuOptions& menuOptions) {
  
    vec2d_f target_angles = math.CalculateAngles(player_pos, enemy_pos);
    HumanizedMoveMouse(target_angles.x, target_angles.y, player_yaw, player_pitch, menuOptions);
}

void Aimbot::HumanizedMoveMouse(float target_yaw, float target_pitch, float current_yaw, float current_pitch, const Menu::MenuOptions& menuOptions) {

    double currentAimbotSpeed = menuOptions.AimbotSpeed;
    float yaw_diff = target_yaw - current_yaw;
    float pitch_diff = -(target_pitch - current_pitch);

    // Normalize yaw
    if (yaw_diff > 180)  yaw_diff -= 360;
    if (yaw_diff < -180) yaw_diff += 360;

    double damping_factor = 0.3; //Friction to combat jitter
    //const double G_0 = menuOptions.AimbotSpeed;    // Gravity force
    double W_0 = 20.0;    // Wind force
    //double M_0 = menuOptions.AimbotSpeed; //50.0; //Testing   // Maximum velocity
    double D_0 = 20.0;   // Damping distance

    double dist = std::hypot(yaw_diff, pitch_diff);
    
    if (menuOptions.AimbotDeadzoneReset && m_isSpeedReset) {
        if (std::chrono::steady_clock::now() < m_speedResetUntil) {
            currentAimbotSpeed = menuOptions.AimbotDeadzoneSpeed;
        }
        else {
            m_isSpeedReset = false;
        }
    }

    if (dist < menuOptions.AimbotDeadzoneSize) {

        if (menuOptions.AimbotDeadzoneReset && !m_isSpeedReset) {
            m_isSpeedReset = true;
            m_speedResetUntil = std::chrono::steady_clock::now() + std::chrono::milliseconds(menuOptions.AimbotDeadzoneDuration);
        }
        // close enough, stop moving and reset velocity.
        m_move_v_x = 0;
        m_move_v_y = 0;
        return; 
    }

    m_move_v_x *= damping_factor;
    m_move_v_y *= damping_factor;

    double W_mag = min(W_0, dist);
    if (dist >= D_0) {
        m_move_W_x = m_move_W_x / 1.414 + (rand() % 1000 / 500.0 - 1.0) * W_mag / 1.414;
        m_move_W_y = m_move_W_y / 1.414 + (rand() % 1000 / 500.0 - 1.0) * W_mag / 1.414;
    }
    else {
        m_move_W_x /= 2;
        m_move_W_y /= 2;
    }

    if (dist > 0) {
        m_move_v_x += m_move_W_x + currentAimbotSpeed * yaw_diff / dist;
        m_move_v_y += m_move_W_y + currentAimbotSpeed * pitch_diff / dist;
    }

    double v_mag = std::hypot(m_move_v_x, m_move_v_y);
    if (v_mag > currentAimbotSpeed) {
        m_move_v_x = (m_move_v_x / v_mag) * currentAimbotSpeed;
        m_move_v_y = (m_move_v_y / v_mag) * currentAimbotSpeed;
    }

    int final_dx = static_cast<int>(std::round(m_move_v_x));
    int final_dy = static_cast<int>(std::round(m_move_v_y));

    if (abs(final_dx) > 0 || abs(final_dy) > 0) {
        mouse_event(MOUSEEVENTF_MOVE, final_dx, final_dy, 0, 0);
    }
}