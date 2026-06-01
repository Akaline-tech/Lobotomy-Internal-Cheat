#include "monoSDK.h"
#include <vector>
#include "../data.h"

std::vector<MonoObject*> GetCreatureList()
{
    std::vector<MonoObject*> result;
    if (!player::creatureList_addr) return result;

    uintptr_t listObj = (uintptr_t)player::creatureList_addr;
    uintptr_t itemsArray = *(uintptr_t*)(listObj + 0x10);
    int32_t size = *(int32_t*)(listObj + 0x18);

    if (!itemsArray || size <= 0) return result;

    uintptr_t elementBase = itemsArray + 0x20;
    for (int i = 0; i < size; ++i) {
        MonoObject* elem = *(MonoObject**)(elementBase + i * sizeof(void*));
        if (elem) {
            result.push_back(elem);
        }
    }
    return result;
}