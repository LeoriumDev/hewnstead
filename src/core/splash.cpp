#include <hewnstead/core/splash.hpp>

#include <spdlog/spdlog.h>

namespace hs {

void printSplash() {
    for (const auto& line : SPLASH_LINES) {
        spdlog::info(line);
    }
}

}  // namespace hs
