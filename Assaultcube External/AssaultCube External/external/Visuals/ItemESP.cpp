#include "ItemESP.h"

void DrawItemBorderBox(float x, float y, float w, float h, ImColor color) {
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x + w, y), color, 1.5f);
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + h), color, 1.5f);
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x + w, y + h), ImVec2(x, y + h), color, 1.5f);
    ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x, y + h), ImVec2(x, y), color, 1.5f);
}

ImColor ItemESP::getItemColor(char type) const {
    switch (static_cast<unsigned char>(type)) {
    case 0x3: return SubtleGray;   // "Pistol Ammo"
    case 0x4: return Gray;         // "Weapon Ammo"
    case 0x5: return Orange;       // "Grenades"
    case 0x6: return Green;        // "Health"
    case 0x7: return SkyBlue;      // "Helmet"
    case 0x8: return DeepSkyBlue;  // "Kevlar"
    case 0x9: return Gold;         // "Akimbo"
    case 0xd: return YellowGreen;  // "Flag"
    default:  return Black;        // Default color
    }
}

void ItemESP::drawItemESP(const GetValues::MemoryValues& memoryValues, const Menu::MenuOptions& menuValues, const Mathematics& math) {

    float* matrix = const_cast<float*>(memoryValues.vMatrix);

    for (const auto& item : memoryValues.activeItems) {

        std::vector<float> itemWorldPosVec = { item.worldPos.x, item.worldPos.y, item.worldPos.z };
        float distance = math.GetDistance3D(memoryValues.playerposvec, itemWorldPosVec);

        if (menuValues.ItemDistanceESP && distance > menuValues.ItemMaxDistance) {
            continue;
        }

        bool shouldDraw = false;
        switch (static_cast<unsigned char>(item.type)) {
        case 0x3: shouldDraw = menuValues.ShowPistolAmmo; break;
        case 0x4: shouldDraw = menuValues.ShowWeaponAmmo; break;
        case 0x5: shouldDraw = menuValues.ShowGrenades; break;
        case 0x6: shouldDraw = menuValues.ShowHealth; break;
        case 0x7: shouldDraw = menuValues.ShowHelmet; break;
        case 0x8: shouldDraw = menuValues.ShowKevlar; break;
        case 0x9: shouldDraw = menuValues.ShowAkimbo; break;
        case 0xd: shouldDraw = menuValues.ShowFlag; break;
        default: break;
        }
        if (!shouldDraw) {
            continue;
        }

        vec3d_f itemScreenPos;
        if (math.WorldToScreen(itemWorldPosVec, itemScreenPos, matrix, memoryValues.imguiSizeX, memoryValues.imguiSizeY)) {

            ImColor currentItemColor = this->getItemColor(item.type);
            currentItemColor.Value.w = menuValues.ItemOpacity;

            float boxWidth = 1500 / distance;
            if (boxWidth < 10.0f) boxWidth = 10.0f;
            if (boxWidth > 50.0f) boxWidth = 50.0f;

            float boxX = itemScreenPos.x - (boxWidth / 2.0f);
            float boxY = itemScreenPos.y - (boxWidth / 2.0f);

            ImVec2 nameTextSize = ImGui::CalcTextSize(item.name.c_str());
            ImGui::GetBackgroundDrawList()->AddText(
                ImVec2(itemScreenPos.x - nameTextSize.x / 2.0f, boxY - nameTextSize.y - 2),
                currentItemColor,
                item.name.c_str()
            );

            if (menuValues.ItemBoxESP) {
                DrawItemBorderBox(boxX, boxY, boxWidth, boxWidth, currentItemColor);
            }

            if (menuValues.ItemDistanceESP) {
                int distInt = static_cast<int>(distance);
                char buffer[32];
                snprintf(buffer, sizeof(buffer), "%d", distInt);
                ImVec2 distTextSize = ImGui::CalcTextSize(buffer);

                ImGui::GetBackgroundDrawList()->AddText(ImVec2(itemScreenPos.x - distTextSize.x / 2.0f, boxY + boxWidth + 2), currentItemColor, buffer);
            }
        }
    }
}