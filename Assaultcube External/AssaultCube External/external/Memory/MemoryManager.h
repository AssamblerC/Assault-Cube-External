#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>

class MemoryManager{

    private:
        HANDLE hProc;
        bool CompareMemory(const BYTE* pData, const BYTE* pPattern, const char* pMask);

    public:
        DWORD FindSignature(HANDLE hProc, DWORD startAddress, DWORD endAddress, BYTE* pPattern, const char* pMask, int byteShift);
        DWORD GetModuleBaseAddress(const WCHAR* lpszModuleName, DWORD pId);
        DWORD GetModuleBaseSize(const WCHAR* lpszModuleName, DWORD pId);
};