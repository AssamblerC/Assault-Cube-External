#include "PlayerESP.h"
#include "../Memory/GetValues.h"

void DrawLine(ImVec2 from, ImVec2 to, ImColor color, float thickness = 2.0f) {
    ImGui::GetBackgroundDrawList()->AddLine(from, to, color, thickness);
}

void DrawBorderBox(ImVec2 pos, ImVec2 size, ImColor color, float thickness = 2.0f) {
    DrawLine(pos, ImVec2(pos.x + size.x, pos.y), color, thickness);
    DrawLine(ImVec2(pos.x + size.x, pos.y), ImVec2(pos.x + size.x, pos.y + size.y), color, thickness);
    DrawLine(ImVec2(pos.x + size.x, pos.y + size.y), ImVec2(pos.x, pos.y + size.y), color, thickness);
    DrawLine(ImVec2(pos.x, pos.y + size.y), pos, color, thickness);
}

void DrawCenteredText(ImVec2 pos, const std::string& text, ImColor color) {
    ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
    ImVec2 textPos = ImVec2(pos.x - textSize.x / 2.0f, pos.y);
    ImGui::GetBackgroundDrawList()->AddText(textPos, color, text.c_str());
}


void PlayerESP::drawPlayerESP(const GetValues::MemoryValues& memoryValues, const Menu::MenuOptions& menuValues, const Mathematics& math) {

    float* matrix = const_cast<float*>(memoryValues.vMatrix);
    bool isTeamMode = IsTeamGameMode(memoryValues.gameMode);

    for (int i = 0; i < memoryValues.playercount; i++) {

        vec3d_f enemyScreenPos;
        if (math.WorldToScreen(memoryValues.enemyposvec[i], enemyScreenPos, matrix, memoryValues.imguiSizeX, memoryValues.imguiSizeY)){
            bool shouldDraw = false;

            if (memoryValues.enemyIsDead[i] == 0 && memoryValues.enemyHealth[i] > 0 && memoryValues.enemyHealth[i] < 101)
            {
                if (isTeamMode) {
                    if (memoryValues.playerTeam != memoryValues.enemyTeam[i] && menuValues.EnemyESP) {
                        shouldDraw = true;
                    }
                    else if (memoryValues.playerTeam == memoryValues.enemyTeam[i] && menuValues.TeamESP) {
                        shouldDraw = true;
                    }
                }
                else {
                    if (menuValues.EnemyESP) {
                        shouldDraw = true;
                    }
                }
            }

            if (!shouldDraw) continue;

            if (menuValues.VisibleCheck && !memoryValues.enemyIsVisible[i]) continue;

            ImColor espColor;
            if (isTeamMode && memoryValues.enemyTeam[i] == 1) {
                espColor = colorBlue;
                if (memoryValues.enemyIsVisible[i] && memoryValues.playerTeam != memoryValues.enemyTeam[i])
                    espColor = colorPurple;
            }
            else if (isTeamMode && memoryValues.enemyTeam[i] == 0) {
                espColor = colorRed;
                if (memoryValues.enemyIsVisible[i] && memoryValues.playerTeam != memoryValues.enemyTeam[i])
                    espColor = colorPurple;
            }
            else {
                espColor = memoryValues.enemyIsVisible[i] ? colorPurple : colorRed;
            }

            float distance = math.GetDistance3D(memoryValues.playerposvec, memoryValues.enemyposvec[i]);
            float height = memoryValues.imguiSizeY / (distance * 0.8f);
            float width = height / 2.0f;

            float box_x = enemyScreenPos.x - width / 2.0f;
            float box_y = enemyScreenPos.y + (height * 0.5f);


            DrawBorderBox(ImVec2(box_x, box_y), ImVec2(width, height), espColor);

            float barWidth = 2.0f;
            float barPadding = 2.5f;

            if (menuValues.HealthESP) {
                float health = static_cast<float>(memoryValues.enemyHealth[i]);
                ImColor healthColor = colorGreen;
                if (health <= 50) healthColor = colorOrange;
                if (health <= 25) healthColor = colorRed;

                float healthBarHeight = height * (health / 100.0f);
                ImVec2 barPos = ImVec2(box_x - barWidth - barPadding, box_y);

                ImGui::GetBackgroundDrawList()->AddRectFilled(barPos, ImVec2(barPos.x + barWidth, barPos.y + height), IM_COL32(0, 0, 0, 180));
                ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(barPos.x, barPos.y + height - healthBarHeight), ImVec2(barPos.x + barWidth, barPos.y + height), healthColor);
            }

            if (menuValues.ArmorESP && memoryValues.enemyArmor[i] > 0) {
                float armor = static_cast<float>(memoryValues.enemyArmor[i]);
                float armorBarHeight = height * (armor / 100.0f);
                ImVec2 barPos = ImVec2(box_x - barWidth - barPadding - barWidth - barPadding, box_y);

                ImGui::GetBackgroundDrawList()->AddRectFilled(barPos, ImVec2(barPos.x + barWidth, barPos.y + height), IM_COL32(0, 0, 0, 180));
                ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(barPos.x, barPos.y + height - armorBarHeight), ImVec2(barPos.x + barWidth, barPos.y + height), deepBlue);
            }

            if (menuValues.PlayerDistanceESP) {
                std::string distStr = std::to_string(static_cast<int>(distance)) + "m";
                ImVec2 textPos = ImVec2(box_x + width / 2.0f, box_y + height + 2.0f);
                DrawCenteredText(textPos, distStr, espColor);
            }

            if (menuValues.PlayerUsernameESP) {
                const std::string& name = memoryValues.enemyUsername[i];
                if (!name.empty()) {
                    ImVec2 textPos = ImVec2(box_x + width / 2.0f, box_y + height + 2.0f + 15.0f);
                    DrawCenteredText(textPos, name, espColor);
                }
            }
        }
    }
}