#include <hewnstead/core/window.hpp>
#include <hewnstead/render/imgui_runtime.hpp>

#include <imgui.h>
#include <spdlog/spdlog.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

namespace hs {

ImguiRuntime::ImguiRuntime(Window& window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;  // disable per-launch imgui.ini
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window.raw(), true);
    ImGui_ImplOpenGL3_Init("#version 410 core");

    spdlog::info("ImGui {} initialized", IMGUI_VERSION);
}

ImguiRuntime::~ImguiRuntime() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void ImguiRuntime::beginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void ImguiRuntime::endFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
bool ImguiRuntime::wantCaptureMouse() const {
    return ImGui::GetIO().WantCaptureMouse;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
bool ImguiRuntime::wantCaptureKeyboard() const {
    return ImGui::GetIO().WantCaptureKeyboard;
}

}  // namespace hs
