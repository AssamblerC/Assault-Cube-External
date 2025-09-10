#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdlib>
#include "../Memory/MemoryManager.h"
#include "../offsets.h"
#include <string>
#include "../Utility/ExternMath.h" 

// This is the structure of an pickable entity in the game's memory.
#pragma pack(push, 1)
struct GameEntity {
	short x;            // Offset 0x0
	short y;            // Offset 0x2
	short padding0;        // Offset 0x4
	short z_overHeightmap; // Offset 0x6 Z-offset on top of heightmap
	char type;          // Offset 0x8 item's Type ID
	char padding1[7];   // Offset 0x9 to 0xF unused
	char isSpawned;     // Offset 0x10 Flag: 1 if the item is spawned
	char padding2[7];   // Offset 0x11 to 0x17 to get a Total size of 24 bytes
};
#pragma pack(pop)

inline bool IsTeamGameMode(int gameMode)
{
    switch (gameMode)
    {
        case offsets::aBotTeamDeathmatch:
        case offsets::aBotTeamSurvivor:
        case offsets::aBotTeamOneShot:
        case offsets::aTeamSurvivor:
        case offsets::aCaptureTheFlag:
        case offsets::aTeamOneShot:
        case offsets::aHuntTheFlag:
        case offsets::aTeamKeepTheFlag:
        case offsets::aTeamPistolFrenzy:
        case offsets::aTeamLastSwissStanding:
            return true; // It's a team-based mode

        default:
            return false; // It's a free-for-all or other mode
    }
}

struct ItemInfo {
	std::string name;
	char type;
	vec3d_f worldPos;
};

class GetValues{	
	public:
		struct MemoryValues {

			int imguiSizeX = 0;
			int imguiSizeY = 0;
			bool gameOpen = true;

			int calculatedWindowWidth = 0;
			int calculatedWindowHeight = 0;
			int radarWorldDiameter = 0;

			int gameMode = 0;
			
			int playercount = 0;
			int playerTeam = 0;
			float playerYawAngle = 0;
			float playerPitchAngle = 0;
			std::vector<bool> enemyIsVisible{ false };
			std::vector<float> playerposvec{ 0 };
			std::vector<std::vector<float>> enemyposvec{ 0 };
			std::vector<int> enemyIsDead{ 0 };
			std::vector<int> enemyTeam{ 0 };
			std::vector<int> enemyHealth{ 0 };
			std::vector<int> enemyArmor{ 0 };
			std::vector<float> enemyYawAngle{ 0 };
			std::vector<std::string> enemyUsername{ {} };

			std::vector<ItemInfo> activeItems;

			float vMatrix[16] = {
			1.0f, 0.0f, 0.0f, 0.0f, 
			0.0f, 1.0f, 0.0f, 0.0f, 
			0.0f, 0.0f, 1.0f, 0.0f, 
			0.0f, 0.0f, 0.0f, 1.0f};
		};

		void Cleanup();
		MemoryValues GetMemoryValues(HWND overlay_window);
		HWND hwndGame = 0;
		void InitializeHandle();

	private:
		bool handlesAcquired = false;
		int attemptCount = 0;
		DWORD pID = NULL;  // Declare pID without initializing
		DWORD threadID = NULL;
		HANDLE phandle = NULL;
		//HWND hwnd;
		DWORD baseaddress = NULL;
		DWORD basesize = NULL;
		DWORD pointerPatternPlayerEntity = NULL;
		DWORD PlayerEntity = NULL;
		DWORD pointerViewMatrix = NULL;
		DWORD pointerFrameCounter = NULL;
		DWORD pointerGameMode = NULL;
		DWORD EnemyEntity = NULL;
		MemoryValues memoryValues;
		float GetZCoordinate(short x, short y, short z_overHeightmap, char type);
		int frameCounter = 0;
		std::vector<int> isVisible{ 0 };

		template <typename T>
		T GetMemory(DWORD offset) {
			T value{};
			ReadProcessMemory(phandle, (LPVOID)(offset), &value, sizeof(value), nullptr);
			return value;
		}
};