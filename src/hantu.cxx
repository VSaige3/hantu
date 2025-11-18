#include "hantu.hxx"
#include <imgui.h>
#include <nfd.h>

#include <common/int.h>

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
}

void hantu::init(GLFWwindow* window) {
    NFD_Init();
    editor = ed::CreateEditor();
}

void hantu::destroy() {
    ed::DestroyEditor(editor);
    NFD_Quit();
}
