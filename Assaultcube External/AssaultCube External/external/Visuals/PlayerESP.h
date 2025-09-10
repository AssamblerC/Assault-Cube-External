#pragma once
#define _USE_MATH_DEFINES
#include <cmath> 
#include <vector>
#include <windows.h>
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"
#include "../Utility/ExternMath.h"
#include "../Memory/GetValues.h"
#include "../Overlay/Menu.h"

class PlayerESP{

    public:
        void drawPlayerESP(const GetValues::MemoryValues& memValues, const Menu::MenuOptions& menValues, const Mathematics& math);

    private:
        ImColor colorGreen = ImColor(0.f, 1.f, 0.f, 1.f);
        ImColor colorOrange = ImColor(1.f, 165.f / 255.f, 0.f, 1.f); 
        ImColor colorRed = ImColor(1.f, 0.f, 0.f, 1.f); 
        ImColor colorBlue = ImColor(0.f, 0.f, 1.f, 1.f); 
        ImColor deepBlue = ImColor(0.0f, 0.8f, 1.0f, 1.0f);
        ImColor colorPurple = ImColor(1.0f, 0.0f, 1.0f, 1.0f);

        void DrawHealth(float Health, float Armor, ImVec2 pos, ImVec2 size);
};
