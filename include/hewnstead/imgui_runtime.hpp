#pragma once

namespace hs {

class Window;

class ImguiRuntime {
public:
    explicit ImguiRuntime(Window& window);
    ~ImguiRuntime();

    // No copy, no move (single global ImGui context).
    ImguiRuntime(const ImguiRuntime&) = delete;
    ImguiRuntime& operator=(const ImguiRuntime&) = delete;
    ImguiRuntime(ImguiRuntime&&) = delete;
    ImguiRuntime& operator=(ImguiRuntime&&) = delete;

    // Per-frame helpers. Call beginFrame() before any ImGui::* widget calls;
    // call endFrame() AFTER your scene render and BEFORE swapBuffers.
    void beginFrame();
    void endFrame();

    // Input gating. Input::cursorPosCallback / keyCallback consult these
    // to decide whether to forward an event to the camera or drop it.
    [[nodiscard]] bool wantCaptureMouse() const;
    [[nodiscard]] bool wantCaptureKeyboard() const;
};

}  // namespace hs
