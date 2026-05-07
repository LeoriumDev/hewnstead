#include <hewnstead/mesh.hpp>
#include <hewnstead/shader.hpp>
#include <hewnstead/window.hpp>

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <exception>

int main() {
    try {
        spdlog::info("Hewnstead Stage 1B: triangle in MVP space");

        hs::Window window(1280, 720, "Hewnstead");
        hs::Shader shader("assets/shaders/triangle.vert", "assets/shaders/triangle.frag");
        hs::Mesh triangle = hs::Mesh::triangle();

        // Camera
        const glm::vec3 eye(0.0f, 0.0f, 3.0f);
        const glm::vec3 target(0.0f, 0.0f, 0.0f);
        const glm::vec3 up(0.0f, 1.0f, 0.0f);

        while (!window.shouldClose()) {
            glClearColor(0.10f, 0.10f, 0.12f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // Per-frame MVP (Model matrix is now identity for now)
            const glm::mat4 model = glm::mat4(1.0f);
            const glm::mat4 view = glm::lookAt(eye, target, up);
            const glm::mat4 projection =
                glm::perspective(glm::radians(60.0f), window.aspect(), 0.1f, 1000.0f);

            shader.use();
            shader.setMat4("u_model", model);
            shader.setMat4("u_view", view);
            shader.setMat4("u_projection", projection);
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
