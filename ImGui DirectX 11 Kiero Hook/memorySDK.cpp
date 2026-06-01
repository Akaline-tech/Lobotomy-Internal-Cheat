#include "memorySDK.h"
#include <Psapi.h>
#include <cstring>

namespace Memory
{

    // =========================
    // 获取模块信息
    // =========================
    void GetModuleInfo(const char* module, void** base, size_t* size)
    {
        HMODULE hMod = GetModuleHandleA(module);

        if (!hMod)
        {
            *base = nullptr;
            *size = 0;
            return;
        }

        MODULEINFO info{};
        GetModuleInformation(GetCurrentProcess(), hMod, &info, sizeof(info));

        *base = info.lpBaseOfDll;
        *size = info.SizeOfImage;
    }

    // =========================
    // 解析AOB
    // =========================
    std::vector<PatternByte> ParsePattern(const char* pattern)
    {
        std::vector<PatternByte> bytes;
        const char* current = pattern;

        while (*current)
        {
            if (*current == '?')
            {
                bytes.push_back({ 0, true });

                if (*(current + 1) == '?')
                    current += 3;
                else
                    current += 2;
            }
            else
            {
                BYTE value = (BYTE)strtoul(current, nullptr, 16);
                bytes.push_back({ value, false });

                current += 2;

                if (*current == ' ')
                    current++;
            }
        }

        return bytes;
    }

    // =========================
    // 向上回溯函数入口
    // =========================
    void* FindFunctionStart(void* addr)
    {
        BYTE* p = (BYTE*)addr;

        for (int i = 0; i < 0x100; i++)
        {
            BYTE* cur = p - i;

            // push rbp
            if (cur[0] == 0x55)
                return cur;

            // mov rbp, rsp
            if (cur[0] == 0x48 &&
                cur[1] == 0x8B &&
                cur[2] == 0xEC)
                return cur;

            // sub rsp, xx
            if (cur[0] == 0x48 &&
                cur[1] == 0x83 &&
                cur[2] == 0xEC)
                return cur;
        }

        return addr;
    }

    void* ScanProcess(const char* pattern)
    {
        auto pat = ParsePattern(pattern);

        SYSTEM_INFO sysInfo{};
        GetSystemInfo(&sysInfo);

        BYTE* current =
            (BYTE*)sysInfo.lpMinimumApplicationAddress;

        BYTE* maxAddress =
            (BYTE*)sysInfo.lpMaximumApplicationAddress;

        MEMORY_BASIC_INFORMATION mbi{};

        while (current < maxAddress)
        {
            if (VirtualQuery(current, &mbi, sizeof(mbi)))
            {
                // =========================
                // 只扫已提交内存
                // =========================
                bool committed =
                    mbi.State == MEM_COMMIT;

                // =========================
                // 可执行页
                // =========================
                bool executable =
                    mbi.Protect & PAGE_EXECUTE ||
                    mbi.Protect & PAGE_EXECUTE_READ ||
                    mbi.Protect & PAGE_EXECUTE_READWRITE ||
                    mbi.Protect & PAGE_EXECUTE_WRITECOPY;

                // =========================
                // 排除guard/noaccess
                // =========================
                bool accessible =
                    !(mbi.Protect & PAGE_GUARD) &&
                    !(mbi.Protect & PAGE_NOACCESS);

                if (committed && executable && accessible)
                {
                    void* result =
                        Scan(mbi.BaseAddress,
                            mbi.RegionSize,
                            pattern);

                    if (result)
                        return result;
                }

                current += mbi.RegionSize;
            }
            else
            {
                current += 0x1000;
            }
        }

        return nullptr;
    }

    void* Scan(void* base, size_t size, const char* pattern)
    {
        auto pat = ParsePattern(pattern);
        BYTE* start = (BYTE*)base;

        size_t patSize = pat.size();

        for (size_t i = 0; i < size - patSize; i++)
        {
            bool found = true;

            for (size_t j = 0; j < patSize; j++)
            {
                if (!pat[j].wildcard &&
                    pat[j].value != *(BYTE*)(start + i + j))
                {
                    found = false;
                    break;
                }
            }

            if (found)
            {
                void* hit = start + i;

                // =========================
                // 🔥 关键：向上回溯函数头
                // =========================
                return FindFunctionStart(hit);
            }
        }

        return nullptr;
    }

    void* FindPattern(const char* module, const char* pattern)
    {
        void* base = nullptr;
        size_t size = 0;

        GetModuleInfo(module, &base, &size);

        if (!base || !size)
            return nullptr;

        return Scan(base, size, pattern);
    }
}