#include <memory>

#include <gui_bootstrap.hxx>
#include <layer_imgui.hxx>

int main() {
    gui_app hantu;

    hantu.layers.emplace_back(std::make_unique<layer_imgui>());
    hantu.run("Hantu");
}
