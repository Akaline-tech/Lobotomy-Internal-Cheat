#pragma once
#include "imgui.h"
#include "data.h"

namespace ImGui
{
    bool ToggleSwitch(const char* label, bool* v, float rounding_bg = 15.f, float rounding_knob = 9.5f);
    
}
bool TabButton(const char* label, bool active);
;