#pragma once
#include <windows.h>
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"

// state management for the UI
enum class MainTabs { LegitBot, Visuals, Misc };
enum class LegitSubTabs { AimAssist, TriggerBot };

class Menu {
public:
    struct MenuOptions {
        // ESP
        bool DrawPlayerESP = true;
        bool TeamESP = false;
        bool EnemyESP = true;
        bool VisibleCheck = false;
        bool HealthESP = true;
        bool ArmorESP = true;
        bool PlayerDistanceESP = true;
        bool PlayerUsernameESP = true;

        bool DrawItemESP = true;
        bool ItemDistanceESP = true;
        bool ItemBoxESP = true;
        float ItemMaxDistance = 80.0f;
        float ItemOpacity = 0.3f;
        bool ShowPistolAmmo = false;
        bool ShowWeaponAmmo = true;
        bool ShowGrenades = true;
        bool ShowHealth = true;
        bool ShowHelmet = true;
        bool ShowKevlar = true;
        bool ShowAkimbo = true;
        bool ShowFlag = true;

        // Radar
        bool DrawRadar = true;

        // Aimbot
        bool AimbotEnabled = true;
        bool AimbotVisCheck = true;
        bool AimbotDrawFov = false;
        float AimbotFov = 800.0f;
        float AimbotSpeed = 50.0f;
        float AimbotDeadzoneSize = 0.35f;
        bool AimbotDeadzoneReset = true;
        float AimbotDeadzoneSpeed = 12.0f;
        int AimbotDeadzoneDuration = 250;
        bool TriggerbotEnabled = true;
        bool TriggerbotFovDraw = true;
        float TriggerbotFov = 15.0f;
    };

    Menu(); // Constructor to apply style once
    MenuOptions GetOptions();
    void Draw();
    void ApplyStyle(ImGuiStyle& style);

private:
    MenuOptions options;
    bool menuIsOpen = true;

    enum class MainTabs {
        LegitBot,
        Visuals,
        Radar
    };
    MainTabs activeMainTab = MainTabs::LegitBot;

    enum class LegitSubTabs {
        AimAssist,
        TriggerBot
    };
    LegitSubTabs activeLegitSubTab = LegitSubTabs::AimAssist;

    enum class VisualsSubTabs {
        PlayerESP,
        ItemESP
    };

    VisualsSubTabs activeVisualsSubTab = VisualsSubTabs::PlayerESP;
};