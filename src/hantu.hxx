#include <layer.hxx>

struct hantu : gui_layer {
    bool demo_window_active = true;

    void init(GLFWwindow* window) override;

    void update(GLFWwindow* window) override;

    void render(GLFWwindow* window) override;

    void destroy() override;
};
