#include <layer.hxx>
#include <imgui_node_editor.h>

#include "fileclass.hxx"
#include "ssb.h"

struct hantu : gui_layer {
    // Fileclass handles loading/saving
    struct ssb_file : fileclass {

        ssb_header* get_header();
        u32* get_bytecode();
        ssb_func_entry* func_table();
        char* string_pool();

        bool load_verify() const noexcept override;
    };

    // TODO: Make this a vector, with each file having a window
    ssb_file ssb;

    ax::NodeEditor::EditorContext* editor = nullptr;

    bool show_demo_window = false;

    void init(GLFWwindow* window) override;

    void update(GLFWwindow* window) override;

    void destroy() override;
};
