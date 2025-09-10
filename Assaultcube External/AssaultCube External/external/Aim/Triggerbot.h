#pragma once

#include "../Memory/GetValues.h"
#include "../Overlay/Menu.h"
#include "../Utility/ExternMath.h" 

class Triggerbot {
public:
    void triggerUpdate(const GetValues::MemoryValues& memValues, const Menu::MenuOptions& menuOptions, const Mathematics& math);

private:
    vec3d_f enemyScreenPos{ 0 };
};