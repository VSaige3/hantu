#include "imgui_utils.hxx"
#include <imgui.h>
#include <imgui_internal.h>

namespace ImGui {

bool BeginTitleBar(const char* id) {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    const float height = ImGui::GetFrameHeight();
    const ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar;

    bool active = ImGui::BeginViewportSideBar("MainMenu", viewport, ImGuiDir_Up, height, flags);
    active |= ImGui::BeginMenuBar();

    return active;
}

void EndTitleBar() {
    ImGui::EndMenuBar();
    ImGui::End();
}

} // namespace ImGui
