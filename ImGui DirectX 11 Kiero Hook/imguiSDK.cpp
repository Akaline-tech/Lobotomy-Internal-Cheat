#include "imguiSDK.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "data.h"
#include <algorithm>
#include "util.h"

static float ImSmootherStep(float current, float target, float speed)
{
    return current + (target - current) * speed;
}

bool ImGui::ToggleSwitch(const char* label,bool* v,float rounding_bg,float rounding_knob)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (!window || window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    float height = GetFrameHeight();
    float width = height * 2.2f * 0.85f * 1.03f;

    ImVec2 pos = window->DC.CursorPos;

    ImRect bb(pos, ImVec2(pos.x + width, pos.y + height));
    ItemSize(bb, style.FramePadding.y);

    if (!ItemAdd(bb, id))
        return false;

    bool hovered = false, held = false;
    bool clicked = ButtonBehavior(bb, id, &hovered, &held);

    if (clicked)
        *v = !*v;

    ImGuiStorage* storage = GetStateStorage();

    float anim = storage->GetFloat(id, *v ? 1.0f : 0.0f);
    float target = *v ? 1.0f : 0.0f;

    anim = ImSmootherStep(anim, target, g.IO.DeltaTime * 12.0f);
    storage->SetFloat(id, anim);

    ImVec4 off = style.Colors[ImGuiCol_FrameBg];
    ImVec4 on = style.Colors[ImGuiCol_CheckMark];

    ImVec4 bg = off;

    if (*v || anim > 0.001f)
    {
        bg.x = off.x + (on.x - off.x) * anim;
        bg.y = off.y + (on.y - off.y) * anim;
        bg.z = off.z + (on.z - off.z) * anim;
        bg.w = off.w + (on.w - off.w) * anim;
    }

    if (hovered)
    {
        bg.x += 0.05f;
        bg.y += 0.05f;
        bg.z += 0.05f;
    }

    ImDrawList* draw = window->DrawList;

    draw->AddRectFilled(bb.Min,bb.Max,GetColorU32(bg),rounding_bg);

    float knob_r = rounding_knob;

    float padding = knob_r * 0.25f;

    float usable_width = bb.GetWidth() - (padding * 2.0f + knob_r * 2.0f);

    float x = bb.Min.x + padding + knob_r + usable_width * anim;

    ImVec2 c(x, bb.Min.y + height * 0.5f);

    draw->AddCircleFilled(c,knob_r,IM_COL32(255, 255, 255, 255));

    if (held)
    {
        draw->AddCircleFilled(c,knob_r * 0.85f,IM_COL32(255, 255, 255, 40));
    }

    ImVec2 label_pos(bb.Max.x + 8.0f,bb.Min.y + (height - GetTextLineHeight()) * 0.5f);

    const char* text_end = FindRenderedTextEnd(label);

    draw->AddText(label_pos, GetColorU32(ImGuiCol_Text), label, text_end);

    return clicked;
}

bool TabButton(const char* label, bool active)
{
    if (active)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.48f, 0.67f, 0.34f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.48f, 0.67f, 0.34f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.48f, 0.67f, 0.34f, 1.00f));
    }

    bool pressed = ImGui::Button(label, ImVec2(140, 45));

    if (active)
    {
        ImGui::PopStyleColor(3);
    }

    return pressed;
}
