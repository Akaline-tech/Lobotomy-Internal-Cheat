#include "game.h"
#include <Windows.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <vector>
#include <iostream>
#include <monoSDK.h>
#include <MinHook.h>
#include "data.h"
#include "util.h"
#include <getData.h>

std::mutex g_matrixLock;
std::mutex g_zombieLock;

namespace
{
    std::thread g_worker;
    std::atomic<bool> g_running{ false };
    std::atomic<bool> g_stopRequested{ false };
    std::mutex g_mutex;
    std::condition_variable g_cv;
    std::queue<std::string> g_queue;
}

namespace GameBackend
{
    void Start()
    {
        bool expected = false;
        if (!g_running.compare_exchange_strong(expected, true)) return;
        g_stopRequested.store(false);

        g_worker = std::thread([]()
            {
                AllocConsole();

                FILE* f;
                freopen_s(&f, "CONOUT$", "w", stdout);

                std::cout << "[+] console initialized" << std::endl;

                MH_Initialize();

                std::cout << "[+] waiting mono..." << std::endl;

                while (!GetModuleHandleA("mono-2.0-bdwgc.dll") && !GetModuleHandleA("mono.dll"))
                    Sleep(100);

                std::cout << "[+] mono found" << std::endl;

                if (!InitMono())
                {
                    std::cout << "[!] InitMono failed" << std::endl;
                    return;
                }

                std::cout << "[+] mono initialized" << std::endl;

                monodata::domain = mono_get_root_domain();

                while (!monodata::domain)
                {
                    monodata::domain = mono_get_root_domain();
                    Sleep(100);
                }

                std::cout << "[+] mono domain: 0x" << std::hex << monodata::domain << std::dec << std::endl;

                mono_thread_attach(monodata::domain);

                std::cout << "[+] mono thread attached" << std::endl;

                mono_assembly_foreach(AssemblyCallback, nullptr);

                if (!g_AssemblyCSharp)
                {
                    std::cout << "[!] Assembly-CSharp not found" << std::endl;
                    return;
                }

                std::cout << "[+] Assembly-CSharp found" << std::endl;

                monodata::image = mono_assembly_get_image(g_AssemblyCSharp);

                if (!monodata::image)
                {
                    std::cout << "[!] image null" << std::endl;
                    return;
                }

                std::cout << "[+] image: 0x" << std::hex << monodata::image << std::dec << std::endl;
                
                player::money_class = mono_class_from_name(monodata::image, "", "MoneyModel");

                player::money_field = mono_class_get_field_from_name(player::money_class, "money");

                player::energy_class = mono_class_from_name(monodata::image, "", "EnergyModel");

                player::energy_field = mono_class_get_field_from_name(player::energy_class, "energy");

                player::GameStatusUI_GameStatusUI_class = mono_class_from_name(monodata::image, "GameStatusUI", "GameStatusUI");

                player::energyContorller_field = mono_class_get_field_from_name(player::GameStatusUI_GameStatusUI_class, "energyContorller");

                player::GameStatusUI_EnergyController_class = mono_class_from_name(monodata::image, "GameStatusUI", "EnergyController");

                player::EnergyMax_field = mono_class_get_field_from_name(player::GameStatusUI_EnergyController_class, "max");

                //异想体

                player::CreatureManager_class = mono_class_from_name(monodata::image, "", "CreatureManager");

                player::CreatureManager_creatureList_field = mono_class_get_field_from_name(player::CreatureManager_class, "creatureList");

                player::creatureModelClass = mono_class_from_name(monodata::image, "", "CreatureModel");

                player::getUnitNameMethod = mono_class_get_method_from_name(player::creatureModelClass, "GetUnitName", 0);

                player::EscapeMethod = mono_class_get_method_from_name(player::creatureModelClass, "Escape", 0);

                player::SuppressedMethod = mono_class_get_method_from_name(player::creatureModelClass, "Suppressed", 0);

                //员工宝贝

                player::AgentManager_class = mono_class_from_name(monodata::image, "", "AgentManager");

                player::AgentManager_agentList_field = mono_class_get_field_from_name(player::AgentManager_class, "agentList");

                player::agentModelClass = mono_class_from_name(monodata::image, "", "AgentModel");


                while (!g_stopRequested.load())
                {
       
                    MonoObject* money_instance = GetStaticFieldObject(player::money_class, "_instance");
                    if (money_instance)
                    {
                        mono_field_get_value(money_instance, player::money_field, &player::lobPoints);
                    }

                    MonoObject* energy_instance = GetStaticFieldObject(player::energy_class, "_instance");
                    if (energy_instance)
                    {
                        mono_field_get_value(energy_instance, player::energy_field, &player::energy);
                    }

                    MonoObject* ui_instance = GetStaticFieldObject(player::GameStatusUI_GameStatusUI_class, "_window");
                    if (ui_instance)
                    {
                        MonoObject* controller_instance = nullptr;

                        mono_field_get_value(ui_instance, player::energyContorller_field, &controller_instance);

                        if (controller_instance)
                        {
                            mono_field_get_value(controller_instance, player::EnergyMax_field, &player::energyMax);
                        }
                    }
                    //异想体
                    MonoObject* CreatureManager_instance = GetStaticFieldObject(player::CreatureManager_class, "_instance");

                    if (CreatureManager_instance)
                    {
                        mono_field_get_value(CreatureManager_instance, player::CreatureManager_creatureList_field, &player::creatureList_addr);
                    }

                    //员工
                    MonoObject* AgentManager_instance = GetStaticFieldObject(player::AgentManager_class, "_instance");

                    if (AgentManager_instance)
                    {
                        mono_field_get_value(AgentManager_instance, player::AgentManager_agentList_field, &player::agentList_addr);
                    }

                    if (feature::AutoSuppressed) {
                        static int tick = 0;
                        if (++tick > 60) {
                            tick = 0;

                            if (player::SuppressedMethod) {
                                // 静态缓存字段，只查找一次
                                static MonoClassField* stateField = mono_class_get_field_from_name(
                                    player::creatureModelClass, "_state");

                                // 建议缓存异想体列表，不要每帧重建
                                static auto creatures = GetCreatureList(); // 如果返回的是静态容器
                                for (auto& creatureObj : creatures) {
                                    int state = 0;
                                    mono_field_get_value(creatureObj, stateField, &state);

                                    // 假设 2 是出逃状态（根据你的游戏版本可能不同，可自行调整）
                                    if (state == 2) {
                                        MonoException* exc = nullptr;
                                        mono_runtime_invoke(player::SuppressedMethod, creatureObj, nullptr, &exc);
                                        Sleep(2); // 可选：避免一帧内大量镇压
                                    }
                                }
                            }
                        }
                    }

                    Sleep(13);
                }

                g_running.store(false);
            });
    }

    void Stop()
    {
        g_stopRequested.store(true);
        g_cv.notify_all();
        if (g_worker.joinable()) g_worker.join();
        std::lock_guard<std::mutex> lk(g_mutex);
        while (!g_queue.empty()) g_queue.pop();
    }

    void Enqueue(const std::string& data)
    {
        std::lock_guard<std::mutex> lk(g_mutex);
        g_queue.push(data);
        g_cv.notify_one();
    }

    bool IsRunning() { return g_running.load(); }
}