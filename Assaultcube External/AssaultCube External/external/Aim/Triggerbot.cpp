#include "Triggerbot.h"
#include "../Memory/GetValues.h"
#include <Windows.h>
#include "../imgui/imgui.h"

void Triggerbot::triggerUpdate(const GetValues::MemoryValues& memoryValues, const Menu::MenuOptions& manuValues, const Mathematics& math)
{
    if (!manuValues.TriggerbotEnabled && !manuValues.TriggerbotFovDraw) {
        return;
    }

    vec3d_f crosshairPos = { memoryValues.imguiSizeX / 2, memoryValues.imguiSizeY / 2, 0.0f };
    auto drawList = ImGui::GetBackgroundDrawList();
    bool isTeamMode = IsTeamGameMode(memoryValues.gameMode);

    for (int i = 0; i < memoryValues.playercount; i++){

        if (!(memoryValues.enemyIsDead[i] == 0)){

            continue;
        }

        bool isTargetableByTeam = !isTeamMode || (memoryValues.enemyTeam[i] != memoryValues.playerTeam);
        if (memoryValues.enemyIsDead[i] != 0 || !isTargetableByTeam || !memoryValues.enemyIsVisible[i]) {
            continue;
        }

        const float* matrix = memoryValues.vMatrix;
        float distance = memoryValues.enemyposvec[i][0] * matrix[3] +
            memoryValues.enemyposvec[i][1] * matrix[7] +
            memoryValues.enemyposvec[i][2] * matrix[11] +
            matrix[15];

        float scaledFov = 12.0f;

        if (distance > 0.1f) {

            const float fovScalingFactor = 35.0f;
            scaledFov = (manuValues.TriggerbotFov * fovScalingFactor) / distance;
        }

        const float minFov = 5.0f;
        const float maxFov = 20.0f;
        scaledFov = (std::max)(minFov, (std::min)(scaledFov, maxFov));

        if (math.WorldToScreen(memoryValues.enemyposvec[i], enemyScreenPos, const_cast<float*>(memoryValues.vMatrix), memoryValues.imguiSizeX, memoryValues.imguiSizeY))
        {

            float distanceCrossEnemy = math.GetDistance2D(crosshairPos, enemyScreenPos);

            if (manuValues.TriggerbotFovDraw) {
                drawList->AddCircleFilled(ImVec2(enemyScreenPos.x, enemyScreenPos.y), scaledFov, IM_COL32(255, 255, 0, 100));
            }

            if (distanceCrossEnemy < scaledFov && GetAsyncKeyState(VK_RBUTTON) & 0x8000)
            {
                if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
                    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                    Sleep(1);
                    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                }
                return;
            }
        }
    }
}