#pragma once
#include <Windows.h>

// 菜单显示标志
extern bool g_showMenu;

// 界面绘制函数
void MenuGui();
void ItemList();
// 辅助控件：带高亮的标签按钮
bool TabButton(const char* label, bool active);