#pragma once
#include <cstdint>
#include <vector>
#include <mutex>
#include <imgui.h>

struct Vector3
{
    float x, y, z;
};

struct Vector2
{
    float x, y;
};

namespace GameData
{
    inline bool matrixReady = false;
    inline int screenWidth = 0;
    inline int screenHeight = 0;
}

struct MonoArray
{
    void* klass;
    void* monitor;
    void* bounds;
    int max_length;
    void* vector;
};

struct CreatureModel {
    void* obj;
    int64_t id;
    std::wstring name;
};

namespace monodata
{
    inline void* domain = nullptr;
    inline void* image = nullptr;
    inline void* coreImage = nullptr;

    inline void* sceneGameClass = nullptr;
    inline void* sceneGame = nullptr;

}

namespace player
{
	inline void* money_class = nullptr;
    inline void* money_field = nullptr;
    inline void* money_instance = nullptr;

    inline void* energy_class = nullptr;
    inline void* energy_field = nullptr;
    inline void* energy_instance = nullptr;

	inline void* GameStatusUI_GameStatusUI_class = nullptr;
    inline void* energyContorller_field = nullptr;
    inline void* GameStatusUI_GameStatusUI_instance = nullptr;
    inline void* GameStatusUI_energyContorller_instance = nullptr;

    inline void* GameStatusUI_EnergyController_class = nullptr;
    inline void* EnergyMax_field = nullptr;

    inline int lobPoints = 0;
    inline float energy = 0;
    inline float energyMax = 0;

    inline void* CreatureManager_class = nullptr;
    inline void* CreatureManager_instance = nullptr;
    inline void* CreatureManager_creatureList_field = nullptr;
    inline void* creatureList_addr = nullptr;
    inline void* creatureModelClass = nullptr;

    inline void* AgentManager_class = nullptr;
    inline void* AgentManager_instance = nullptr;
    inline void* AgentManager_agentList_field = nullptr;
    inline void* agentList_addr = nullptr;
    inline void* agentModelClass = nullptr;

	inline void* getUnitNameMethod = nullptr;
    inline void* EscapeMethod = nullptr;
    inline void* SuppressedMethod = nullptr;
}

namespace feature
{
	inline int lobPoints = 0;
    inline float energyPoints = 0.f;
    inline float movementMul = 1.f;

	inline bool AutoSuppressed = false;
}
