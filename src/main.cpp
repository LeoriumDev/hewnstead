#include <hewnstead/mesh.hpp>
#include <hewnstead/shader.hpp>
#include <hewnstead/window.hpp>

#include <glad/gl.h>

#include <spdlog/spdlog.h>

#include <cstdlib>
#include <exception>

int main() {
    try {
        spdlog::info("Hewnstead Stage 1A Part 2: triangle");

        hs::Window window(1280, 720, "Hewnstead");
        hs::Shader shader("assets/shaders/triangle.vert", "assets/shaders/triangle.frag");
        hs::Mesh triangle = hs::Mesh::triangle();

        while (!window.shouldClose()) {
            glClearColor(0.10f, 0.10f, 0.12f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            shader.use();
            triangle.draw();

            window.swapBuffers();
            window.pollEvents();
        }
    } catch (const std::exception& e) {
        spdlog::critical("Fatal: {}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
