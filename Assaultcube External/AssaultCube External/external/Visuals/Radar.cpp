#include "radar.h"
#include "../Memory/GetValues.h"
#include <algorithm> 

void Radar::drawRadar(const GetValues::MemoryValues& memoryValues, const Menu::MenuOptions& menuValues, const Mathematics& math) {

    if (!menuValues.DrawRadar) {
        return;
    }
    const ImVec2 windowSize = ImGui::GetIO().DisplaySize;

    const float liveVirtualWidth = memoryValues.calculatedWindowWidth;
    const float constantVirtualHeight = 1800.0f; // The constant vertical anchor of the game's UI -> Ghidra

    if (liveVirtualWidth <= 0 || constantVirtualHeight <= 0) {
        return;
    }

    const float scale = windowSize.y / constantVirtualHeight; //Calc a scale, my best guess from what I found in Ghidra
    const float effectiveVirtualWidth = (std::min)(liveVirtualWidth, 1800.0f); // Logic is directly from the decompiled game code.
    const float virtualRadarRadius = (effectiveVirtualWidth / 5.0f) * 0.5f;
    const float virtualRadarCenterX = (liveVirtualWidth - virtualRadarRadius) - 72.0f;
    const float virtualRadarCenterY = virtualRadarRadius + 64.0f;

    const float screenRadarCenterX = virtualRadarCenterX * scale;
    const float screenRadarCenterY = virtualRadarCenterY * scale;

    const float playerViewAngleYaw = memoryValues.playerYawAngle * (static_cast<float>(M_PI) / 180.0f);
    const bool isTeamMode = IsTeamGameMode(memoryValues.gameMode);
    const float worldToRadarScale = (virtualRadarRadius * 2.0f) / memoryValues.radarWorldDiameter;

    for (int i = 0; i < memoryValues.playercount; i++) {

        float deltaX_world = memoryValues.enemyposvec[i][0] - memoryValues.playerposvec[0];
        float deltaY_world = memoryValues.enemyposvec[i][1] - memoryValues.playerposvec[1];

        float deltaX_v = deltaX_world * worldToRadarScale;
        float deltaY_v = deltaY_world * worldToRadarScale;

        if ((deltaX_v * deltaX_v) + (deltaY_v * deltaY_v) > (virtualRadarRadius * virtualRadarRadius)) {
            continue;
        }

        std::vector<float> nonrotatedVectors = { deltaX_v, -deltaY_v };
        std::vector<float> rotatedVectors = math.rotateVector(nonrotatedVectors, -playerViewAngleYaw);

        float deltaX_s = rotatedVectors[0] * scale;
        float deltaY_s = rotatedVectors[1] * scale;
        ImVec2 playerIconPos(screenRadarCenterX + deltaX_s, screenRadarCenterY + deltaY_s);


        float enemyViewAngleYaw = memoryValues.enemyYawAngle[i] * (static_cast<float>(M_PI) / 180.0f);
        float relativeYaw = enemyViewAngleYaw - playerViewAngleYaw;

        std::vector<float> P1 = math.GetNextP(playerIconPos.x, playerIconPos.y, relativeYaw - (90.f * M_PI / 180.f), 7.5f);
        std::vector<float> P2 = math.GetNextP(playerIconPos.x, playerIconPos.y, relativeYaw - (225.f * M_PI / 180.f), 5.5f); 
        std::vector<float> P3 = math.GetNextP(playerIconPos.x, playerIconPos.y, relativeYaw - (-45.f * M_PI / 180.f), 5.5f); 

        if (memoryValues.enemyIsDead[i] == 0 && math.GetDistance2D(memoryValues.playerposvec, memoryValues.enemyposvec[i]) < memoryValues.radarWorldDiameter && memoryValues.enemyHealth[i] > 0 && memoryValues.enemyHealth[i] < 101) {

            ImColor radarColor;
            if (isTeamMode && memoryValues.enemyTeam[i] == 1) {
                radarColor = memoryValues.enemyIsVisible[i] ?  colorPurple :  colorBlue;
            }
            else if (isTeamMode && memoryValues.enemyTeam[i] == 0) {
                radarColor = memoryValues.enemyIsVisible[i] ?  colorPurple :  colorRed;
            }
            else {
                radarColor = memoryValues.enemyIsVisible[i] ? colorPurple : colorRed;
            }

            if (isTeamMode && memoryValues.playerTeam != memoryValues.enemyTeam[i]) {
                ImGui::GetBackgroundDrawList()->AddTriangleFilled(ImVec2(P1[0], P1[1]), ImVec2(P2[0], P2[1]), ImVec2(P3[0], P3[1]), radarColor);
            }
            else if (!isTeamMode) {
                ImGui::GetBackgroundDrawList()->AddTriangleFilled(ImVec2(P1[0], P1[1]), ImVec2(P2[0], P2[1]), ImVec2(P3[0], P3[1]), radarColor);
            }
        }
    }
}