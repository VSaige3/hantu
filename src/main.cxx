#include <stdlib.h>
#include <stdio.h>
#include <memory>

#include <gui_bootstrap.hxx>
#include <layer_imgui.hxx>
#include <common/logging.h>
#include <common/int.h>

#include "hantu.hxx"
#include "version.h"

int main(int argc, char** argv) {
    bool help = false;
    bool version = false;

    for (u32 i = 0; i < argc; i++) {
        const char* arg = argv[i];
        help    |= (strcmp("--help", arg) == 0);
        version |= (strcmp("--version", arg) == 0);
    }

    if (version) {
        printf("%d.%d.%d\n", version_major, version_minor, version_patch);
        return EXIT_SUCCESS;
    }

    if (help) {
        printf("Hantu SSB editor v%d.%d.%d\n", version_major, version_minor, version_patch);
        printf("Open-source @ %s\n", git_url);
        printf("Created by:\n");
        for (const char* author : authors) {
            printf("  - %s\n", author);
        }
        return EXIT_SUCCESS;
    }

    gui_app han;

    han.layers.emplace_back(std::make_unique<layer_imgui>());
    han.layers.emplace_back(std::make_unique<hantu>());
    han.run("Hantu");
}
