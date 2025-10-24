#include <memory>

#include <gui_bootstrap.hxx>
#include <layer_imgui.hxx>
#include "hantu.hxx"

int main() {
    gui_app han;

    han.layers.emplace_back(std::make_unique<layer_imgui>());
    han.layers.emplace_back(std::make_unique<hantu>());
    han.run("Hantu");
}
