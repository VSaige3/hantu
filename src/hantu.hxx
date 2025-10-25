#include <layer.hxx>
#include <imgui_node_editor.h>

struct hantu : gui_layer {
    ax::NodeEditor::EditorContext* editor = nullptr;

    bool show_demo_window = false;

    void init(GLFWwindow* window) override;

    void update(GLFWwindow* window) override;

    void render(GLFWwindow* window) override;

    void destroy() override;
};
