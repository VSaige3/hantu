#include "hantu.hxx"
#include <imgui.h>
#include <common/int.h>

namespace ed = ax::NodeEditor;

void hantu::init(GLFWwindow* window) {
    editor = ed::CreateEditor();
}

void hantu::update(GLFWwindow* window) {
    ImGui::ShowDemoWindow(&demo_window_active);

    ImGui::Begin("Test Editor"); {
        ed::SetCurrentEditor(editor); {
            ed::Begin("Node Editor", ImVec2());

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

            ed::End();
        }
        ed::SetCurrentEditor(nullptr);
    }
    ImGui::End();
}

void hantu::render(GLFWwindow* window) {

}

void hantu::destroy() {
    ed::DestroyEditor(editor);
}
