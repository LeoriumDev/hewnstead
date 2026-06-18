#pragma once
#include <spdlog/spdlog.h>

#include <chrono>
#include <string_view>

namespace hs {

class ScopedTimer {
public:
    explicit ScopedTimer(std::string_view name)
        : m_name(name), m_start(std::chrono::steady_clock::now()) {}

    ~ScopedTimer() {
        auto ms =
            std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - m_start)
                .count();
        spdlog::info("[profile] {}: {:.2f} ms", m_name, ms);
    }

private:
    std::string_view m_name;
    std::chrono::steady_clock::time_point m_start;
};

}  // namespace hs
