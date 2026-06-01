#pragma once
#include <Windows.h>
#include <vector>

namespace Memory
{
    struct PatternByte
    {
        BYTE value;
        bool wildcard;
    };

    void GetModuleInfo(const char* module, void** base, size_t* size);

    std::vector<PatternByte> ParsePattern(const char* pattern);

    void* Scan(void* base, size_t size, const char* pattern);

    void* ScanProcess(const char* pattern);

    void* FindPattern(const char* module, const char* pattern);
}