cmake_minimum_required(VERSION 3.13)
project(hantu
    VERSION 1.0.0
    LANGUAGES C CXX
)

# We need compile_commands.json for the include paths in some editors
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

set(CMAKE_C_STANDARD 11) # C11 gives us static_assert()
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Add ImGui as a library
add_library(imgui STATIC
    lib/imgui/imgui.cpp
    lib/imgui/imgui_demo.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_tables.cpp
    lib/imgui/imgui_widgets.cpp
    lib/imgui/backends/imgui_impl_opengl3.cpp
    lib/imgui/backends/imgui_impl_glfw.cpp
    lib/imgui/misc/cpp/imgui_stdlib.cpp
)

add_library(imgui_node_editor
    lib/imgui_node_editor/imgui_node_editor.cpp
    lib/imgui_node_editor/imgui_node_editor_api.cpp
    lib/imgui_node_editor/imgui_canvas.cpp
    lib/imgui_node_editor/crude_json.cpp
)
include_directories(lib/imgui lib/imgui_node_editor lib/bobtail)

set(COREGUI_USE_IMGUI ON)
add_subdirectory(lib/coregui)
add_subdirectory(lib/bobtail)

add_executable(hantu
    src/main.cxx
    src/hantu.cxx
)

# Be careful editing this, coregui must come before ImGui. I thought the order
# didn't matter, but it apparently does. Basically, coregui has a source file
# dependent on ImGui, but we provide the copy of ImGui.
target_link_libraries(hantu PUBLIC coregui imgui imgui_node_editor)
