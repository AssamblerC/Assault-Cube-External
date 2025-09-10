#pragma once
#define _USE_MATH_DEFINES
#include <cmath> 
#include <vector>
#include <windows.h>
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"
#include "../Memory/GetValues.h"
#include "../Overlay/Menu.h"
#include "../Utility/ExternMath.h"

class Radar {
	public:
		void drawRadar(const GetValues::MemoryValues& memValues, const Menu::MenuOptions& menValues, const Mathematics& math);

	private:
		ImColor colorRed = ImColor(1.f, 0.f, 0.f, 1.f);
		ImColor colorBlue = ImColor(0.f, 0.f, 1.f, 1.f);
		ImColor colorPurple = ImColor(1.0f, 0.0f, 1.0f, 1.0f);
};