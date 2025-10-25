#include <layer.hxx>
#include <imgui_node_editor.h>

struct hantu : gui_layer {
    ax::NodeEditor::EditorContext* editor = nullptr;

    bool demo_window_active = true;

    void init(GLFWwindow* window) override;

    void update(GLFWwindow* window) override;

    void render(GLFWwindow* window) override;

    void destroy() override;
};
