#pragma once
#include <string>
#include <imgui.h>
#include <common/int.h>

/// std::string::append(), but with printf() formatting syntax.
/// Also appends a newline after the message.
void str_format_append(std::string& output, const char* format_str, ...);

/// @brief Define an explcitly sized variant of ImGui::InputScalar()
#define IMPL_IMGUI_SCALAR_INPUT(T, U)                                                            \
static bool Input##T(const char* label, U* data, U step, U step_fast,                            \
                     const char* format, ImGuiInputTextFlags flags) {                            \
    return ImGui::InputScalar(label, ImGuiDataType_##T, data, &step, &step_fast, format, flags); \
}                                                                                                \

namespace ImGui {
    bool BeginTitleBar(const char* id = "MainMenuBar");
    void EndTitleBar();

    /// @brief A sort of backwards assert that displays a message in the GUI
    ///
    /// @param condition The failure condition. If this is *true*, your custom
    /// message is displayed using ImGui::Text, along with a line asking the
    /// user to report the issue.
    /// @param format A format string for sprintf()
    /// @param ... Format string arguments
    void PlsReportIf(bool condition, const char* format, ...);

    /// @brief Text input box for a Phantom Dust encoded string
    ///
    /// This is for the Phantom Dust text format that encodes 6 characters into
    /// a 32-bit integer. It's commonly used in SSB and ALR files.
    /// @param label Label text to describe the input box
    /// @param text1 The encoded 32-bit integer to modify
    /// @param text2 Optional encoded integer, allowing up to 12 characters total
    /// @return Whether the text was edited
    bool InputPDString(const char* label, u32* text1, u32* text2 = nullptr);

    void PDString(u32 text1, u32 text2 = 0);

    // Define ImGui::InputScalar() variants with explicit sizes
    IMPL_IMGUI_SCALAR_INPUT(U8, u8)
    IMPL_IMGUI_SCALAR_INPUT(S8, s8)
    IMPL_IMGUI_SCALAR_INPUT(U16, u16)
    IMPL_IMGUI_SCALAR_INPUT(S16, s16)
    IMPL_IMGUI_SCALAR_INPUT(U32, u32)
    IMPL_IMGUI_SCALAR_INPUT(S32, s32)
    IMPL_IMGUI_SCALAR_INPUT(U64, u64)
    IMPL_IMGUI_SCALAR_INPUT(S64, s64)
} // namespace ImGui
