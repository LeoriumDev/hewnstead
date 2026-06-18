#pragma once

#include <utility>

namespace hs {

// DeleteFn(0) must be a no-op (true for all glDelete*)
template <auto DeleteFn>
class GlHandle {
public:
    GlHandle() = default;

    explicit GlHandle(unsigned int id) : m_id(id) {}

    ~GlHandle() { DeleteFn(m_id); }

    GlHandle(const GlHandle&) = delete;
    GlHandle& operator=(const GlHandle&) = delete;

    GlHandle(GlHandle&& other) noexcept : m_id(std::exchange(other.m_id, 0)) {}

    GlHandle& operator=(GlHandle&& other) noexcept {
        if (this != &other) {
            DeleteFn(m_id);
        }
        m_id = std::exchange(other.m_id, 0);
        return *this;
    }

    [[nodiscard]] unsigned int get() const { return m_id; }

private:
    unsigned int m_id = 0;
};

}  // namespace hs
