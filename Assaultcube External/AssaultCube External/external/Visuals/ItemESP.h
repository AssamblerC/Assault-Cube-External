#pragma once
#include "../Memory/GetValues.h"
#include "../Utility/ExternMath.h"
#include "../Overlay/Menu.h" 
#include "../imgui/imgui.h"

class ItemESP {
public:
    void drawItemESP(const GetValues::MemoryValues& memValues, const Menu::MenuOptions& menValues, const Mathematics& math);

private:
    ImColor getItemColor(char type) const;

    ImColor SubtleGray = ImColor(0.4f, 0.4f, 0.4f, 1.0f);
    ImColor Gray = ImColor(0.85f, 0.85f, 0.85f, 1.0f);
    ImColor Orange = ImColor(1.0f, 0.5f, 0.0f, 1.0f);
    ImColor Green = ImColor(0.0f, 1.0f, 0.0f, 1.0f);
    ImColor SkyBlue = ImColor(0.5f, 0.8f, 0.9f, 1.0f);
    ImColor DeepSkyBlue = ImColor(0.0f, 0.8f, 1.0f, 1.0f);
    ImColor Gold = ImColor(1.0f, 0.84f, 0.0f, 1.0f);
    ImColor Black = ImColor(0.0f, 0.0f, 0.0f, 1.0f);
    ImColor YellowGreen{ 0.60f, 0.80f, 0.20f, 1.0f };
};