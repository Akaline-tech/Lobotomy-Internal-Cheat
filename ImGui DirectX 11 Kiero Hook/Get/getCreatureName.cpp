#include <monoSDK.h>
#include "../data.h"

char* GetCreatureName(MonoObject* creatureObj)
{
    if (!creatureObj || !player::getUnitNameMethod) return nullptr;

    MonoException* exc = nullptr;
    MonoObject* nameObj = mono_runtime_invoke(player::getUnitNameMethod, creatureObj, nullptr, &exc);
    if (exc || !nameObj) {
        std::cout << "GetUnitName failed" << std::endl;
        return nullptr;
    }

    std::string nameUtf8 = ReadMonoStringUtf8(nameObj);
    if (nameUtf8.empty()) return nullptr;

    char* result = new char[nameUtf8.size() + 1];
    strcpy(result, nameUtf8.c_str());
    return result;
}
//需要释放内存 delete[] name;