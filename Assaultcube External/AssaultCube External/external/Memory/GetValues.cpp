#include "GetValues.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

void GetValues::InitializeHandle(){

    phandle = NULL;
    handlesAcquired = false;
    attemptCount = 0;
    hwndGame = 0;

    while (attemptCount < 120) {

        const wchar_t gamemodule[] = L"ac_client.exe";
        hwndGame = FindWindow(nullptr, L"AssaultCube");
        if (hwndGame != 0){
            threadID = GetWindowThreadProcessId(hwndGame, &pID); // &pID is the address of the uninitialized DWORD
            // Now pID contains the process ID and threadID contains the thread ID
            phandle = OpenProcess(PROCESS_VM_READ, FALSE, pID); //PROCESS_ALL_ACCESS | PROCESS_VM_READ | PROCESS_VM_WRITE
        }

        if (phandle != NULL && phandle != INVALID_HANDLE_VALUE) {

            handlesAcquired = true; // Set flag to true if handles are valid

            MemoryManager memory;

            baseaddress = memory.GetModuleBaseAddress(gamemodule, pID);
            basesize = memory.GetModuleBaseSize(gamemodule, pID);
            pointerPatternPlayerEntity = memory.FindSignature(phandle, baseaddress, basesize, offsets::patternPlayerEntity, offsets::maskPlayerEntity, 2);//2->Byteshift from xx to ??
            pointerFrameCounter = memory.FindSignature(phandle, baseaddress, basesize, offsets::patternFrameCounter, offsets::maskFrameCounter, 1);
            pointerGameMode = memory.FindSignature(phandle, baseaddress, basesize, offsets::patternGameMode, offsets::maskGameMode, 7);
            pointerViewMatrix = memory.FindSignature(phandle, baseaddress, basesize, offsets::patternViewMatrix, offsets::maskViewMatrix, 7);

            break;
        }
        if (phandle == INVALID_HANDLE_VALUE) {
            CloseHandle(phandle);
            phandle = NULL;
        }

        // Wait for 5 seconds before trying again, with 120 attempts it ends up waiting 10 Minutes for Gamestart
        Sleep(5000); 

        attemptCount++;
    }
}

std::string GetItemName(char type) {
    switch (static_cast<unsigned char>(type)) {
    case 0x3: return "Pistol";
    case 0x4: return "Ammo";
    case 0x5: return "Grenades";
    case 0x6: return "Health";
    case 0x7: return "Helmet";
    case 0x8: return "Kevlar";
    case 0x9: return "Akimbo";
    case 0xd: return "CTF";
    default:  return "";
    }
}

float GetValues::GetZCoordinate(short x, short y, short z_overHeightmap, char type) {
    DWORD heightmapDataBase = 0;
    unsigned char mapShiftValue = 0;
    char heightBaseMapValue = 0;

    heightmapDataBase = GetMemory<DWORD>(baseaddress + offsets::heightmapDataPtr);
    mapShiftValue = GetMemory<unsigned char>(baseaddress + offsets::mapShiftValue);

    if (heightmapDataBase == 0) {
        return 0.0f; 
    }

    // Copied straight out of Ghidra, replicate what the game does to get the base Z Value of map 
    DWORD heightBaseMapValueAddress = heightmapDataBase + 1 + ((y << (mapShiftValue & 0x1F)) + x) * 0x10;
    heightBaseMapValue = GetMemory<char>(heightBaseMapValueAddress);
    // The base Z Value needs to be adjusted to map terrain, except the Flag
    float heightRealMapValue;

    if (static_cast<unsigned char>(type) == 0xd) {
        heightRealMapValue = static_cast<float>(heightBaseMapValue);
    }
    else {
        heightRealMapValue = static_cast<float>(heightBaseMapValue) + (static_cast<float>(z_overHeightmap) * 0.1f);
    }

    return heightRealMapValue;
}

void GetValues::Cleanup() {
    if (phandle != NULL && phandle != INVALID_HANDLE_VALUE) {
        CloseHandle(phandle);
        phandle = NULL;
    }
}

GetValues::MemoryValues GetValues::GetMemoryValues(HWND overlay_window) {

    RECT target_rect{};
    int width = 0;
    int height = 0;

    if (IsWindow(this->hwndGame) && IsWindow(overlay_window)) {

        RECT game_client_rect;
        GetClientRect(this->hwndGame, &game_client_rect);

        POINT game_top_left = { game_client_rect.left, game_client_rect.top };
        ClientToScreen(this->hwndGame, &game_top_left);

        int game_width = game_client_rect.right - game_client_rect.left;
        int game_height = game_client_rect.bottom - game_client_rect.top;

        memoryValues.imguiSizeX = game_width;
        memoryValues.imguiSizeY = game_height;

        RECT overlay_rect;
        GetWindowRect(overlay_window, &overlay_rect);

        if (overlay_rect.left != game_top_left.x ||
            overlay_rect.top != game_top_left.y ||
            (overlay_rect.right - overlay_rect.left) != game_width ||
            (overlay_rect.bottom - overlay_rect.top) != game_height)
        {
            MoveWindow(overlay_window, game_top_left.x, game_top_left.y, game_width, game_height, TRUE);
        }
    }
    else if (!IsWindow(this->hwndGame)) {
        memoryValues.gameOpen = false;
    }

    if (handlesAcquired) {

        ReadProcessMemory(phandle, (LPCVOID)GetMemory<DWORD>(pointerViewMatrix), &memoryValues.vMatrix, sizeof(memoryValues.vMatrix), nullptr);
        PlayerEntity = GetMemory<DWORD>(pointerPatternPlayerEntity);
        EnemyEntity = GetMemory<DWORD>(PlayerEntity + offsets::EnemyEntity);//can't be found before a game started, so do it always.
        frameCounter = GetMemory<int>(GetMemory<DWORD>(pointerFrameCounter));

        memoryValues.gameMode = GetMemory<int>(GetMemory<DWORD>(pointerGameMode));
        memoryValues.playercount = GetMemory<int>(PlayerEntity + offsets::PlayerCount);
        memoryValues.playerTeam = GetMemory<int>(GetMemory<DWORD>(PlayerEntity) + offsets::TeamNum);
        memoryValues.playerYawAngle = GetMemory<float>(GetMemory<DWORD>(PlayerEntity) + offsets::Yaw);
        memoryValues.playerPitchAngle = GetMemory<float>(GetMemory<DWORD>(PlayerEntity) + offsets::Pitch);

        memoryValues.radarWorldDiameter = GetMemory<int>(baseaddress + offsets::radarWorldDiameter);
        memoryValues.calculatedWindowWidth = GetMemory<int>(baseaddress + offsets::calculatedWindowWidth);

        memoryValues.playerposvec.resize(offsets::offsetsXYZHead.size());
        memoryValues.enemyIsDead.resize(memoryValues.playercount);
        memoryValues.enemyTeam.resize(memoryValues.playercount);
        memoryValues.enemyHealth.resize(memoryValues.playercount);
        memoryValues.enemyArmor.resize(memoryValues.playercount);
        memoryValues.enemyYawAngle.resize(memoryValues.playercount);
        memoryValues.enemyposvec.resize(memoryValues.playercount, std::vector<float>(offsets::offsetsXYZHead.size()));
        memoryValues.enemyUsername.resize(memoryValues.playercount);
        memoryValues.enemyIsVisible.resize(memoryValues.playercount);

        isVisible.resize(memoryValues.playercount);
        memoryValues.activeItems.clear();

        DWORD entityListPtr = GetMemory<DWORD>(baseaddress + offsets::itemEntityList);
        int entityCount = GetMemory<int>(baseaddress + offsets::itemEntityCount);

        if (entityListPtr != 0 && entityCount > 0){
            std::vector<GameEntity> entities(entityCount);
            ReadProcessMemory(phandle, (LPCVOID)entityListPtr, entities.data(), entityCount * sizeof(GameEntity), nullptr);

            for (const auto& entity : entities){

                if (entity.isSpawned == 1){

                    std::string name = GetItemName(entity.type);

                    if (!name.empty()){

                        ItemInfo item;
                        item.name = name;
                        item.type = entity.type;
                        item.worldPos.x = static_cast<float>(entity.x);
                        item.worldPos.y = static_cast<float>(entity.y);
                        item.worldPos.z = GetZCoordinate(entity.x, entity.y, entity.z_overHeightmap, entity.type) +1.35f;//1.35 Offset from heightmapbase + overHeightmap -> to lazy to find the real value
                        memoryValues.activeItems.push_back(item);
                    }
                }
            }
        }

        if (memoryValues.playercount > 1) {

            for (int i = 0; i < memoryValues.playercount; i++) {

                DWORD enemyBase = GetMemory<DWORD>(EnemyEntity + i * offsets::playerCountDistance);

                char usernameBuffer[32] = {};
                ReadProcessMemory(phandle, (LPCVOID)(enemyBase + offsets::Username), &usernameBuffer, sizeof(usernameBuffer) - 1, nullptr);
                memoryValues.enemyUsername[i] = std::string(usernameBuffer);

                memoryValues.enemyIsDead[i] = GetMemory<int>(enemyBase + offsets::IsDead);
                memoryValues.enemyTeam[i] = GetMemory<int>(enemyBase + offsets::TeamNum);
                memoryValues.enemyHealth[i] = GetMemory<int>(enemyBase + offsets::Health);
                memoryValues.enemyArmor[i] = GetMemory<int>(enemyBase + offsets::Armor);
                memoryValues.enemyYawAngle[i] = GetMemory<float>(enemyBase + offsets::Yaw);
                isVisible[i] = GetMemory<int>(enemyBase + offsets::Visible);

                memoryValues.enemyIsVisible[i] = isVisible[i] >= frameCounter - 10;

                for (int j = 0; j < offsets::offsetsXYZHead.size(); j++) {

                    memoryValues.enemyposvec[i][j] = GetMemory<float>(GetMemory<DWORD>(EnemyEntity + i * offsets::playerCountDistance) + offsets::offsetsXYZHead[j]);
                    memoryValues.playerposvec[j] = GetMemory<float>(GetMemory<DWORD>(PlayerEntity) + offsets::offsetsXYZHead[j]);
                }
            }
        }
        else {
            for (int j = 0; j < offsets::offsetsXYZHead.size(); j++) {

                memoryValues.playerposvec[j] = GetMemory<float>(GetMemory<DWORD>(PlayerEntity) + offsets::offsetsXYZHead[j]);
            }
        };

        return memoryValues;
    }

    else {
        GetValues::Cleanup();
        exit(EXIT_FAILURE);
    }
}