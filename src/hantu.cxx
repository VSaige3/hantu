#include "hantu.hxx"
#include <imgui.h>
#include <nfd.h>

#include <common/int.h>

#include "imgui_utils.hxx"
#include "ssb.h"

namespace ed = ax::NodeEditor;

ssb_header* hantu::ssb_file::get_header() {
    return (ssb_header*)data;
}

u32* hantu::ssb_file::get_bytecode() {
    auto* header = (ssb_header*)data;
    return &header->instructions[0];
}

ssb_func_entry* hantu::ssb_file::func_table() {
    const auto* header = (ssb_header*)data;
    return (ssb_func_entry*)(data + header->func_table_addr);
}

u32 hantu::ssb_file::num_functions() {
    const auto func_ptr = (intptr_t)func_table();
    const auto string_ptr = (intptr_t)string_pool();
    return std::abs(string_ptr - func_ptr) / sizeof(ssb_func_entry);
}

char* hantu::ssb_file::string_pool() {
    const auto* header = (ssb_header*)data;
    return (char*)(data + header->string_pool_ptr);
}

bool hantu::ssb_file::load_verify() const noexcept {
    const auto* header = (ssb_header*)data;
    bool passed = true;
    passed &= header->header_size == sizeof(*header);
    passed &= header->string_pool_ptr == header->string_pool_ptr2;

    return passed;
}

void do_title_bar(hantu& han) {
    const bool ctrl_pressed = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl);
    bool load_ssb = ctrl_pressed && ImGui::IsKeyPressed(ImGuiKey_L, false);
    if (ImGui::BeginTitleBar()) {
        if (ImGui::BeginMenu("File")) {
            load_ssb |= ImGui::MenuItem("Load SSB", "Ctrl-L");
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
}

void hantu::update(GLFWwindow* window) {
    do_title_bar(*this);

    if (show_demo_window) {
        ImGui::ShowDemoWindow(&show_demo_window);
    }

    ImGui::Begin("Test Editor"); {
        ed::SetCurrentEditor(editor); {
            ed::Begin("Node Editor", ImVec2()); {
                u32 id = 1;
                // Start drawing nodes.
                ed::BeginNode(id++);
                    ImGui::Text("Node A");
                    ed::BeginPin(id++, ed::PinKind::Input);
                        ImGui::Text("-> In");
                    ed::EndPin();
                    ImGui::SameLine();
                    ed::BeginPin(id++, ed::PinKind::Output);
                        ImGui::Text("Out ->");
                    ed::EndPin();
                ed::EndNode();
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
        for (u32 i = 0; i < ssb.num_functions(); i++) {
            const ssb_func_entry& entry = functions[i];
            decoded_text name = decode_text(entry);
            ImGui::Text("%s @ 0x%X", name.data, entry.func_offset);
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
