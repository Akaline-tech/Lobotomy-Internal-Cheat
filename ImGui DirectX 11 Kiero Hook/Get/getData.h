#pragma once
#include <monoSDK.h>
#include <mutex>

std::vector<MonoObject*> GetCreatureList();
char* GetCreatureName(MonoObject* creatureObj);
std::vector<MonoObject*> GetAgentList();
char* GetAgentName(MonoObject* agentObj);