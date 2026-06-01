//Gui.cpp
#include "includes.h"
#include "game.h"
#include <Windows.h>
#include <d3d11.h>
#include "data.h"
#include "monoSDK.h"
#include "imguiSDK.h"
#include "Gui.h"
#include <mutex>
#include <getData.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present oPresent = nullptr;
HWND window = nullptr;
WNDPROC oWndProc = nullptr;
ID3D11Device* pDevice = nullptr;
ID3D11DeviceContext* pContext = nullptr;
ID3D11RenderTargetView* mainRenderTargetView = nullptr;
bool init = false;

void CreateRenderTarget(IDXGISwapChain* pSwapChain)
{
    if (mainRenderTargetView) return;
    ID3D11Texture2D* pBackBuffer = nullptr;
    if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer)))
    {
        pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &mainRenderTargetView);
        pBackBuffer->Release();
    }
}

void CleanupRenderTarget()
{
    if (mainRenderTargetView) { mainRenderTargetView->Release(); mainRenderTargetView = nullptr; }
}

ImFont* g_FontDefault = nullptr;
ImFont* g_FontTitle = nullptr;

void InitImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;
    io.Fonts->Clear();

    static const ImWchar ranges[] = {
    0x0020, 0x00FF,
    0x3000, 0x30FF,
    0x4E00, 0x9FFF,
    0xFF08, 0xFF09,
    0
    };

    ImFontConfig fontConfigNormal;
    fontConfigNormal.PixelSnapH = true;
    fontConfigNormal.OversampleH = 2;
    fontConfigNormal.OversampleV = 2;

    g_FontDefault = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\msyh.ttc",16.0f,&fontConfigNormal,ranges);

    if (!g_FontDefault) {
        g_FontDefault = io.Fonts->AddFontDefault();
    }

    io.Fonts->Build();

    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX11_Init(pDevice, pContext);

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 10.0f;
    style.FrameRounding = 8.0f;
    style.ChildRounding = 8.0f;
    style.GrabMinSize = 14.0f;
    style.GrabRounding = 999.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.10f, 0.65f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.09f, 0.09f, 0.10f, 0.65f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.09f, 0.09f, 0.10f, 1.00f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.09f, 0.10f, 0.65f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.09f, 0.09f, 0.10f, 0.65f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.09f, 0.09f, 0.10f, 0.65f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.14f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.18f, 0.18f, 0.19f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.22f, 0.22f, 0.23f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.16f, 0.16f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.22f, 0.22f, 0.23f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.66f, 0.77f, 0.33f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.16f, 0.16f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.22f, 0.22f, 0.23f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.76f, 0.87f, 0.33f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.76f, 0.87f, 0.33f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.82f, 0.93f, 0.38f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.76f, 0.87f, 0.33f, 1.00f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.25f, 0.25f, 0.26f, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.76f, 0.87f, 0.33f, 1.00f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.82f, 0.93f, 0.38f, 1.00f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.22f, 0.22f, 0.23f, 0.60f);
    style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.91f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.52f, 1.00f);
}

LRESULT __stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    ImGuiIO& io = ImGui::GetIO();
    if (g_showMenu)
    {
        ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
        if (io.WantCaptureMouse)
            return true;
    }
    return CallWindowProc(oWndProc, hWnd, msg, wParam, lParam);
}

HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    if (!init)
    {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
        {
            pDevice->GetImmediateContext(&pContext);
            DXGI_SWAP_CHAIN_DESC sd{};
            pSwapChain->GetDesc(&sd);
            window = sd.OutputWindow;
            CreateRenderTarget(pSwapChain);
            oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
            InitImGui();
            init = true;
        }
        else
            return oPresent(pSwapChain, SyncInterval, Flags);
    }

    static bool monoAttached = false;
    if (!monoAttached && monodata::domain) {
        mono_thread_attach(monodata::domain);
        monoAttached = true;
    }

    if (!mainRenderTargetView)
    {
        CreateRenderTarget(pSwapChain);
        if (!mainRenderTargetView)
            return oPresent(pSwapChain, SyncInterval, Flags);
    }

    static bool lastInsState = false;
    bool currentInsState = GetAsyncKeyState(VK_INSERT) & 1;
    if (currentInsState && !lastInsState)
        g_showMenu = !g_showMenu;
    lastInsState = currentInsState;

    ImGuiIO& io = ImGui::GetIO();
    GameData::screenWidth = io.DisplaySize.x;
    GameData::screenHeight = io.DisplaySize.y;
    io.MouseDrawCursor = g_showMenu;
    if (!g_showMenu) ShowCursor(FALSE);

    static bool prevShowMenu = false;
    if (g_showMenu != prevShowMenu)
    {
        if (g_showMenu) {
            if (set_lockState && set_visible) {
                UnlockCursor();
                SetCursorVisible(true);
            }
            ShowCursor(TRUE);
        }
        else {
            if (set_lockState && set_visible) {
                LockCursor();
                SetCursorVisible(false);
            }
        }
        prevShowMenu = g_showMenu;
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (g_showMenu) {
        ClipCursor(nullptr);
        MenuGui();
    }

    ImDrawList* draw = ImGui::GetForegroundDrawList();

    draw->AddLine(ImVec2(8, 8), ImVec2(8, 21), IM_COL32(255, 200, 200, 255), 1.5f);
	draw->AddText(g_FontDefault, 0.f, ImVec2(11, 6), IM_COL32(255, 255, 255, 255), u8"Astral For Lobotomy | By Liya1337");
    


    ImGui::Render();
    pContext->OMSetRenderTargets(1, &mainRenderTargetView, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return oPresent(pSwapChain, SyncInterval, Flags);
}

int tab = 0;
void MenuGui() {
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(1920, 1080), ImGuiCond_Once);
    ImGui::SetNextWindowBgAlpha(0.f);
    ImGui::Begin(u8"   ", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::End();

    ImGui::SetNextWindowSize(ImVec2(850, 500), ImGuiCond_Once);
  
    ImGui::Begin(u8" Astral", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    ImGui::BeginChild("tabs", ImVec2(160, 0), true);
    if (TabButton(u8"全局", tab == 0)) tab = 0;
    if (TabButton(u8"异想体", tab == 1)) tab = 1;
    if (TabButton(u8"员工", tab == 2)) tab = 2;
    if (TabButton(u8"杂项", tab == 3)) tab = 3;
    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::BeginChild("content", ImVec2(0, 0), false);

    if (tab == 0)
    {
        ImGui::Text(u8"全局");
        ImGui::Spacing();
        ImGui::BeginChild("全局child", ImVec2(328, 0), true);

        ImGui::Text(u8"点数");
        ImGui::Separator();
        if (ImGui::Button(u8"设置LOB点数")) {
            MonoObject* money_instance = GetStaticFieldObject(player::money_class, "_instance");
            mono_field_set_value(money_instance, player::money_field, &feature::lobPoints);
        }
        ImGui::SameLine();
        ImGui::InputInt("##lobPoints", &feature::lobPoints);

        if (ImGui::Button(u8"设置能源点数")) {
            MonoObject* energy_instance = GetStaticFieldObject(player::energy_class, "_instance");
            mono_field_set_value(energy_instance, player::energy_field,  &feature::energyPoints);
        }
        ImGui::SameLine();
        ImGui::InputFloat("##energyPoints", &feature::energyPoints);

        ImGui::Separator();

        if (ImGui::Button(u8"跳过这一天(full energy)")) {
            MonoObject* energy_instance = GetStaticFieldObject(player::energy_class, "_instance");
            mono_field_set_value(energy_instance, player::energy_field, &player::energyMax);
        }


        ImGui::EndChild();
        ImGui::SameLine();
        ImGui::BeginChild("全局child2", ImVec2(328, 0), true);

        ImGui::Text(u8"LOB点数 %d", player::lobPoints);
        ImGui::Text(u8"Energy %.2f", player::energy);
        ImGui::Text(u8"EnergyMax %.2f", player::energyMax);

        

        ImGui::EndChild();
    }
    else if (tab == 1)
    {
        ImGui::Text(u8"异想体");
        ImGui::Spacing();
        ImGui::BeginChild("异想体child", ImVec2(328, 0), true);
        
		ImGui::ToggleSwitch(u8"自动镇压", &feature::AutoSuppressed);

        ImGui::EndChild();
        ImGui::SameLine();
        ImGui::BeginChild("异想体child2", ImVec2(328, 0), true);

        ImGui::Text(u8"注意! 部分异想体在强制突破时会导致游戏崩溃,例如樱下墓。");
        ImGui::Text(u8"同一异想体可以多次强制突破但不建议这样做。");
		ImGui::Separator();
        static int creatureIndex = 0;
        for (auto* creatureObj : GetCreatureList()) {
            char* name = GetCreatureName(creatureObj);
            if (name) {
                ImGui::Text(u8"异想体: %s", name);
				ImGui::SameLine();
                ImGui::PushID(creatureIndex);
                if (ImGui::Button(u8"突破##escape", ImVec2(60, 20))) {
                    if (player::EscapeMethod) {
                        MonoException* exc = nullptr;
                        mono_runtime_invoke(player::EscapeMethod, creatureObj, nullptr, &exc);
                        if (exc) {
                            std::cout << "Escape call failed" << std::endl;
                        }
                        else {
                            std::cout << "Escape invoked" << std::endl;
                        }
                    }
                }
                ImGui::PopID();
                creatureIndex++;
                ImGui::SameLine();
                ImGui::PushID(creatureIndex);
                if (ImGui::Button(u8"镇压##Suppressed", ImVec2(60, 20))) {
                    if (player::SuppressedMethod) {
                        MonoException* exc = nullptr;
                        mono_runtime_invoke(player::SuppressedMethod, creatureObj, nullptr, &exc);
                        if (exc) {
                            std::cout << "Suppressed call failed" << std::endl;
                        }
                        else {
                            std::cout << "Suppressed invoked" << std::endl;
                        }
                    }
                }
                ImGui::PopID();
                creatureIndex++;
                delete[] name;
            }
        }
        creatureIndex = 0;
        ImGui::EndChild();
    }
    else if (tab == 2)
    {
        ImGui::Text(u8"员工");
        ImGui::Spacing();
        ImGui::BeginChild("员工child", ImVec2(328, 0), true);

        if (ImGui::Button(u8"设置员工移动速度")) {
            for (auto* agentObj : GetAgentList()) {
                static MonoClassField* movementMulField = nullptr;
                movementMulField = mono_class_get_field_from_name(player::agentModelClass, "movementMul");
                mono_field_set_value(agentObj, movementMulField, &feature::movementMul);
            }
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::InputFloat("##movementMul", &feature::movementMul);

        ImGui::EndChild();
        ImGui::SameLine();
        ImGui::BeginChild("员工child2", ImVec2(328, 0), true);
        static int agentIndex = 0;
        for (auto* agentObj : GetAgentList()) {
            char* name = GetAgentName(agentObj);
            if (name) {
                ImGui::Text(u8"员工: %s", name);
                ImGui::SameLine();
                static MonoClassField* hpField = nullptr;
                hpField = mono_class_get_field_from_name(player::agentModelClass, "hp");
				static float hp = 0;
                mono_field_get_value(agentObj, hpField, &hp);

                ImGui::Text(u8"血量: %2.f", hp);
                delete[] name;
            }
        }

        ImGui::EndChild();
    }
    else if (tab == 3)
    {
        ImGui::Text(u8"杂项");
        ImGui::Spacing();
        ImGui::BeginChild("杂项child", ImVec2(328, 0), true);
        ImGui::EndChild();
        ImGui::SameLine();
        ImGui::BeginChild("杂项child2", ImVec2(328, 0), true);
        ImGui::EndChild();
    }

    ImGui::EndChild();
    ImGui::End();
}
