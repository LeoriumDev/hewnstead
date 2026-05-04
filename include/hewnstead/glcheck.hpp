#pragma once

#include <string_view>

namespace hs {

// Drains the GL error queue, logging each error via spdlog.
// Call after any cluster of GL state changes you suspect.
// In debug, prefer the GL_CHECK() macro which captures __FILE__/__LINE__.
void glCheckError(std::string_view file, int line);

}  // namespace hs

#ifndef NDEBUG
    #define GL_CHECK() ::hs::glCheckError(__FILE__, __LINE__)
#else
    #define GL_CHECK() ((void)0)
#endif
