#include <layer.hxx>
#include <imgui_node_editor.h>

#include "fileclass.hxx"
#include "ssb.h"
#include "decompiler.hxx"

struct hantu : gui_layer {
    // Fileclass handles loading/saving
    
    // TODO: Make this a vector, with each file having a window
    ssb_file ssb;

    FunctionDecompiler *decompiler = nullptr;

    const ssb_func_entry *selected_function = nullptr;

    ax::NodeEditor::EditorContext* editor = nullptr;

    bool show_demo_window = false;

    void init(GLFWwindow* window) override;

    void update(GLFWwindow* window) override;

    void destroy() override;
};
