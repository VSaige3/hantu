#include "imgui_utils.hxx"
#include <imgui.h>
#include <imgui_internal.h>

#include "pd_common.h"

void str_format_append(std::string& output, const char* format_str, ...) {
    char buf[2048] = {0};

    // We use helpers from stdarg.h to handle the variadic (...) arguments.
    va_list arg_list = {};
    va_start(arg_list, format_str);
    const int return_code = vsnprintf(buf, sizeof(buf) - 1, format_str, arg_list);
    va_end(arg_list);

    output.append(buf);
    output.append("\n");
}

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

void PlsReportIf(bool condition, const char* format, ...) {
    if (!condition) {
        // Failure condition wasn't hit, everything's fine.
        return;
    }

    // We use helpers from stdarg to handle the variadic (...) arguments.
    char msg[8192] = {0};
    va_list arg_list = {};
    va_start(arg_list, format);
    const int return_code = vsnprintf(msg, sizeof(msg) - 1, format, arg_list);
    va_end(arg_list);

    if (strlen(format) > sizeof(msg) - 1) {
        ImGui::Text("%s(): [Programmer error] message was too long to display.\n", __func__);
    }

    ImGui::Text("WARNING: %s", msg);
    ImGui::Text("Please report this so I can research it.");
}

bool InputPDString(const char* label, u32* text1, u32* text2) {
    if (text1 == nullptr) {
        // We need something to edit...
        return false;
    }

    u8 size = ENCODED_CHAR_COUNT;
    decoded_text buf = {0};
    decode_single32(buf.data, *text1);
    if (text2 != nullptr) {
        // Also decode the next value
        decode_single32(&buf.data[ENCODED_CHAR_COUNT], *text2);

        // With a second value, we can store twice as many characters.
        size *= 2;
    }

    ImGui::SetNextItemWidth(ImGui::CalcTextSize("1").x * (size + 8));

    // TODO: Look into using a character filter callback to only allow the
    // characters that can be encoded.
    const bool edited = ImGui::InputText(label, buf.data, size + 1);

    if (edited) {
        // We need to re-encode the text
        *text1 = encode_single32(buf.data);
        if (text2 != nullptr) {
            *text2 = encode_single32(&buf.data[ENCODED_CHAR_COUNT]);
        }
    }

    return edited;
}

void PDString(u32 text1, u32 text2) {
    u8 size = ENCODED_CHAR_COUNT;
    decoded_text buf = decode_double(text1, text2);
    if (text2 != 0) {
        // With a second value, we can store twice as many characters.
        size *= 2;
    }

    ImGui::SetNextItemWidth(ImGui::CalcTextSize("1").x * (size + 8));
    ImGui::Text("%s", buf.data);
}
} // namespace ImGui
