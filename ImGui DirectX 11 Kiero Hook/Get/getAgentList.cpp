#include "monoSDK.h"
#include <vector>
#include "../data.h"

std::vector<MonoObject*> GetAgentList()
{
    std::vector<MonoObject*> result;
    if (!player::agentList_addr) return result;

    uintptr_t listObj = (uintptr_t)player::agentList_addr;
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