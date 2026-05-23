#include <hewnstead/core/application.hpp>
#include <hewnstead/core/splash.hpp>

#include <spdlog/spdlog.h>

#include <cstdlib>
#include <exception>

int main() {
    try {
        hs::printSplash();
        hs::Application app;
        app.run();
    } catch (const std::exception& e) {
        spdlog::critical("Fatal: {}", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
