#include <monoSDK.h>
#include "../data.h"

char* GetAgentName(MonoObject* agentObj)
{
    if (!agentObj || !player::agentModelClass) return nullptr;

    static MonoClassField* nameField = nullptr;
    nameField = mono_class_get_field_from_name(player::agentModelClass, "name");
    if (!nameField) return nullptr;

    MonoObject* nameObj = nullptr;
    mono_field_get_value(agentObj, nameField, &nameObj);
    if (!nameObj) return nullptr;

    std::string utf8 = ReadMonoStringUtf8(nameObj);
    if (utf8.empty()) return nullptr;

    char* result = new char[utf8.size() + 1];
    strcpy(result, utf8.c_str());
    return result;
}