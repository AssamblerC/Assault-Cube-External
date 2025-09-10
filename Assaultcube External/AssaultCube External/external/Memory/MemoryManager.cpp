#include "MemoryManager.h"

DWORD MemoryManager::GetModuleBaseAddress(const WCHAR* lpszModuleName, DWORD pId) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pId);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    MODULEENTRY32 ModuleEntry32;
    ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hSnapshot, &ModuleEntry32)) {
        do {
            if (_wcsicmp(ModuleEntry32.szModule, lpszModuleName) == 0) {
                CloseHandle(hSnapshot);
                return (DWORD)ModuleEntry32.modBaseAddr;
            }
        } while (Module32Next(hSnapshot, &ModuleEntry32));
    }

    CloseHandle(hSnapshot);
    return 0; 
}

DWORD MemoryManager::GetModuleBaseSize(const WCHAR* lpszModuleName, DWORD pId) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pId);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    MODULEENTRY32 ModuleEntry32;
    ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hSnapshot, &ModuleEntry32)) {
        do {
            if (_wcsicmp(ModuleEntry32.szModule, lpszModuleName) == 0) {
                CloseHandle(hSnapshot);
                return ModuleEntry32.modBaseSize;
            }
        } while (Module32Next(hSnapshot, &ModuleEntry32));
    }

    CloseHandle(hSnapshot);
    return 0; 
}

bool MemoryManager::CompareMemory(const BYTE* pData, const BYTE* pPattern, const char* pMask) {
    for (; *pMask; ++pMask, ++pData, ++pPattern) {
        if (*pMask == 'x' && *pData != *pPattern) {
            return false;
        }
    }
    return (*pMask) == NULL;
}

DWORD MemoryManager::FindSignature(HANDLE hProc, DWORD startAddress, DWORD sizeAddress, BYTE* pPattern, const char* pMask, int byteShift) {
    BYTE* buffer = new BYTE[sizeAddress];

    if (!ReadProcessMemory(hProc, reinterpret_cast<LPCVOID>(startAddress), buffer, sizeAddress, NULL)) {
        delete[] buffer;
        return 0;
    }

    for (DWORD i = 0; i < sizeAddress; ++i) {
        if (CompareMemory(buffer + i, pPattern, pMask)) {
            DWORD foundAddress = startAddress + i + byteShift;
            delete[] buffer;
            return foundAddress;
        }
    }

    delete[] buffer;
    return 0;
}