#pragma once
#include <Windows.h>
#include <vector>

namespace offsets
{
    //ac_client.exe = 0x400000

	//static int ViewMatrix = 0x57DFD0;
    static BYTE patternViewMatrix[] = { 0x0F, 0x28, 0xCB, 0xF3, 0x0F, 0x59, 0x0D, 0x00, 0x00, 0x00, 0x00, 0xF3, 0x0F, 0x58, 0xD0 };
    static const char* maskViewMatrix = "xxxxxxx????xxxx";

	//static int FrameCounter = 0x57FF8C; 
    static BYTE patternFrameCounter[] = { 0xA3, 0x00, 0x00, 0x00, 0x00, 0x8A, 0x44, 0x24, 0x1C };
    static const char* maskFrameCounter = "x????xxxx";

    //static int GameMode = 0x58ABF8;
    static BYTE patternGameMode[] = { 0x0F, 0x85, 0x89, 0x05, 0x00, 0x00, 0xA1, 0x00, 0x00, 0x00, 0x00, 0xC6, 0x05, 0x00, 0x00, 0x00, 0x00, 0x01 };
    static const char* maskGameMode = "xxxxxxx????xx????x";
    enum teamGameModes {
        // Bots
        aBotTeamDeathmatch = 7,
        aBotTeamSurvivor = 20,
        aBotTeamOneShot = 21,
        // Multiplayer
        aTeamSurvivor = 4,
        aCaptureTheFlag = 5,
        aTeamOneShot = 11,
        aHuntTheFlag = 13,
        aTeamKeepTheFlag = 14,
        aTeamPistolFrenzy = 16,
        aTeamLastSwissStanding = 17,
    };

    //DWORD PlayerEntity = 0x58AC00;
    static BYTE patternPlayerEntity[] = { 0x8B, 0x35, 0x00, 0x00, 0x00, 0x00, 0x3B, 0xEE };
    static const char* maskPlayerEntity = "xx????xx";

    static int EnemyEntity = 0x04; // 0x18AC04; 
    static int PlayerCount = 0x0C; // 0x18AC0C;

    //Offsets per Entity
    static int Username = 0x205; //(String)
    static int TeamNum = 0x30C; //(int)
    static int Health = 0xEC; //int
    static int Armor = 0xF0; //int
    static int IsDead = 0x318; //(int)
    static int Visible = 0xE4; //int
    static int Yaw = 0x34; //(float)
    static int Pitch = 0x38; //(float)
    static std::vector<int> offsetsXYZHead = { 0x4, 0x8, 0xC }; //Head Positions(float)
    static std::vector<int> offsetsXYZBody = { 0x2C, 0x30, 0x28 }; //Body Positions(float)
    
    static int playerCountDistance = 0x4; //Distance between each playerentity in memory

    //Too lazy to find patterns, already understood the concept
    static int itemEntityList = 0x19086C;    // Static pointer to the entity list pointer
    static int itemEntityCount = 0x190874;   // Static offset for the number of entities

    static int heightmapDataPtr = 0x182938;  // Pointer to the heightmap data
    static int mapShiftValue = 0x182930;    // The map's shift value for height calculation

    static int radarWorldDiameter = 0x18A8EC;
    static int calculatedWindowWidth = 0x17ED2C;
};
