#include "hantu.hxx"
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <nfd.h>

#include <common/int.h>
#include <common/file.h>

#include "imgui_utils.hxx"
#include "ssb.h"

namespace ed = ax::NodeEditor;

void do_title_bar(hantu& han) {
    const bool ctrl = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl);
    const bool shift = ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift);
    bool load_ssb = ctrl && ImGui::IsKeyPressed(ImGuiKey_L, false);
    bool save_ssb = ctrl && ImGui::IsKeyPressed(ImGuiKey_S, false);
    bool save_as_ssb = ctrl &&shift && ImGui::IsKeyPressed(ImGuiKey_S, false);
    if (ImGui::BeginTitleBar()) {
        if (ImGui::BeginMenu("File")) {
            load_ssb |= ImGui::MenuItem("Load SSB", "Ctrl-L");
            save_ssb |= ImGui::MenuItem("Save SSB", "Ctrl-S");
            save_as_ssb |= ImGui::MenuItem("Save SSB As", "Ctrl-Shift-S");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window")) {
            ImGui::MenuItem("Demo Window", nullptr, &han.show_demo_window);
            ImGui::EndMenu();
        }
        ImGui::EndTitleBar();
    }

    if (load_ssb) {
        // Display the file picker and load the ALR if a file is picked
        char* path = nullptr;
        const nfdu8filteritem_t filters[] = { { "Script Binary", "ssb"} };
        nfdresult_t result = NFD_OpenDialogU8(&path, filters, ARRAY_SIZE(filters), nullptr);
        if (result == NFD_OKAY && path != nullptr) {
            han.ssb.load(path);
        }
        free(path);
    }

    if (save_as_ssb) {
        char* path = nullptr;
        const nfdu8filteritem_t filters[] = { { "Script Binary", "ssb"} };
        const char* default_path = han.ssb.filename.c_str();
        nfdresult_t result = NFD_SaveDialogU8(&path, filters, ARRAY_SIZE(filters), default_path, nullptr);
        if (result == NFD_OKAY && path != nullptr) {
            han.ssb.save(path);
        }
        free(path);
    }

    if (save_ssb) {
        han.ssb.save(han.ssb.filename.c_str());
    }
}

bool first_frame = false;

void do_string_editor(ssb_file& ssb) {
    ImGui::Begin("Strings"); {
        const void* end = ssb.data + ssb.size;
        char* text = ssb.string_pool();

        while (text < end) {
            std::string label = "##" + std::to_string(uintptr_t(text));
            if (ssb.string_pool_edit_dest == text) {
                const s32 buf_size = MAX(0, s32(strlen(text)));
                const u32 height = ImGui::GetTextLineHeight() * 2;
                ImGui::InputTextMultiline(label.c_str(), &ssb.string_pool_input);

                ImGui::SameLine();
                if (ImGui::Button("Apply")) {
                    // Fill with spaces first, to avoid leaving behind old text
                    memset(text, ' ', buf_size);

                    // Copy as much as possible. Without this the rest get set
                    // to NUL and make us think there's a bunch of empty strings.
                    const u32 copy_len = MIN(ssb.string_pool_input.length(), buf_size);
                    strncpy(text, ssb.string_pool_input.c_str(), copy_len);

                    // Reset.
                    ssb.string_pool_input = "";
                    ssb.string_pool_edit_dest = nullptr;
                }
            } else {
                std::string edit_label = "Edit" + label;
                if (ImGui::Button(edit_label.c_str())) {
                    ssb.string_pool_edit_dest = text;
                    ssb.string_pool_input = text;
                }

                ImGui::SameLine();
                ImGui::Text("%s", text);
            }

            text += strlen(text) + 1;
        }
    }
    ImGui::End();
}

void hantu::update(GLFWwindow* window) {
    do_title_bar(*this);

    if (show_demo_window) {
        ImGui::ShowDemoWindow(&show_demo_window);
    }

    ImGui::Begin("Test Editor"); {
        ed::SetCurrentEditor(editor); {
            ed::Begin("Node Editor", ImVec2()); {
                u32 id = 10;

                decompiler.render_all_nodes(id);

                if (first_frame) {
                    ImVec2 v = ImVec2();
                    decompiler.space_nodes(id, v);
                    ed::NavigateToContent(0.5f);
                }

                first_frame = false;

            }
            ed::End();
        }
        ed::SetCurrentEditor(nullptr);
    }
    ImGui::End();

    if (!ssb.data) {
        return;
    }

    ImGui::Begin(ssb.filename.c_str()); {
        const ssb_func_entry* functions = ssb.func_table();
        char label[32];
        for (u32 i = 0; i < ssb.num_functions(); i++) {
            const ssb_func_entry& entry = functions[i];
            decoded_text name = decode_double(entry.text1, entry.text2);
            snprintf(label, 31, "%s @ 0x%X", name.data, entry.func_offset);
            if (ImGui::Selectable(label)) {
                if (selected_function != &entry) {
                    printf("Selected func %s @ 0x%X\n", name.data, entry.func_offset);
                    selected_function = &entry;
                    this->decompiler = FunctionDecompiler(entry.func_offset);
                    if (!(this->decompiler.decompile(-1, ssb.get_bytecode()))) {
                        printf("Error: %s", str_decomp_error(this->decompiler.get_last_error()));
                        this->destroy();
                    }
                    first_frame = true;
                }
            }
        }
    }
    ImGui::End();

    do_string_editor(ssb);
}

void hantu::init(GLFWwindow* window) {
    NFD_Init();
    editor = ed::CreateEditor();

    const char* font_path = "font.ttf";
    if (file_exists(font_path)) {
        ImGuiIO& io = ImGui::GetIO();
        ImFontConfig cfg;
        cfg.MergeMode = true;
        io.Fonts->AddFontFromFileTTF(font_path, 15.0f);
        io.Fonts->AddFontFromFileTTF(font_path, 15.0f, &cfg, io.Fonts->GetGlyphRangesJapanese());
    }
}

void hantu::destroy() {
    ed::DestroyEditor(editor);
    NFD_Quit();
}
