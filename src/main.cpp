#include <hewnstead/window.hpp>

#include <glad/gl.h>

#include <spdlog/spdlog.h>

#include <cstdlib>
#include <exception>

int main() {
    try {
        spdlog::info("Hewnstead Stage 1A Part 1: empty window");

        hs::Window window(1280, 720, "Hewnstead");

        while (!window.shouldClose()) {
            glClearColor(0.10f, 0.10f, 0.12f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            window.swapBuffers();
            window.pollEvents();
        }
    } catch (const std::exception& e) {
        spdlog::critical("Fatal: {}", e.what());
        return EXIT_FAILURE;
    }
}