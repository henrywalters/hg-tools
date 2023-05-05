//
// Created by henry on 4/12/23.
//
#include "game.h"

#include <hagame/graphics/windows.h>
#include <hagame/core/hg.h>
#include <hagame/core/assets.h>

#include "tilemapEditor/tilemapEditor.h"
#include "particleEditor/particleEditor.h"

#if USE_IMGUI
#include "imgui.h"
#include "imgui_demo.cpp"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#endif

using namespace hg::graphics;

hg::Vec2 toVec(ImVec2 vec) {
    return hg::Vec2(vec.x, vec.y);
}

void Game::onInit() {
#if !HEADLESS
    m_window = Windows::Create(name(), m_size);

    Windows::Events.subscribe(WindowEvents::Close, [&](Window* window) {
        running(false);
    });

    Windows::Events.subscribe(WindowEvents::Resize, [&](Window* window) {

    });
#endif

#if USE_IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplGlfw_InitForOpenGL(m_window->window(), true);
    ImGui_ImplOpenGL3_Init("#version 300 es");
#endif

    std::vector<std::string> shaders = {
        "color",
        "sprite",
        "text",
        "particle"
    };

    for (const auto& shader : shaders) {
        hg::loadShader(shader, "shaders/" + shader + ".vert", "shaders/" + shader + ".frag");
    }

}

void Game::onBeforeUpdate() {
#if !HEADLESS
    m_window->clear();
    glfwSwapInterval(0);
#endif

#if USE_IMGUI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
#endif
}

void Game::onAfterUpdate() {
#if USE_IMGUI
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

#if !HEADLESS
    m_window->render();
#endif
}

void Game::onDestroy() {
    // CLEAN UP
}

void Game::onUpdate(double dt) {
#if USE_IMGUI

    ImGui::Begin("HaGame Tools");

    if (ImGui::Button("Tilemap Editor")) {
        auto tool = std::make_shared<TilemapEditor>();
        m_tools.push_back(tool);
        tool->initialize();
    }

    if (ImGui::Button("Particle Editor")) {
        auto tool = std::make_shared<ParticleEditor>();
        m_tools.push_back(tool);
        tool->initialize();
    }

    ImGui::End();

    for (auto& tool : m_tools) {
        bool showWindow = true;

        tool->render(dt);

        ImGui::Begin(tool->name().c_str(), &showWindow, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus);

        tool->renderMenu(dt);

        ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

        auto windowPos = toVec(ImGui::GetWindowPos()) + toVec(ImGui::GetWindowContentRegionMin());

        auto windowSize = ImGui::GetWindowSize();
        float aspectRatio = (float) FULL_HD[0] / FULL_HD[1];
        auto displaySize = ImVec2(windowSize.x, windowSize.x / aspectRatio);

        if (displaySize.y > windowSize.y) {
            displaySize = ImVec2(windowSize.y * aspectRatio, windowSize.y);
        }

        if (ImGui::IsWindowFocused()) {
            tool->keyboardMouse = m_window->input.keyboardMouse;
            tool->keyboardMouse.mouse.position -= windowPos;
            tool->keyboardMouse.mouse.position[1] = displaySize.y - tool->keyboardMouse.mouse.position[1];
            tool->keyboardMouse.mouse.position = tool->keyboardMouse.mouse.position.div(toVec(displaySize)).prod(hg::Vec2(FULL_HD[0], FULL_HD[1]));
            tool->tick();
        }

        ImGui::Image((void*) tool->getDisplay()->id, displaySize, ImVec2(0, 1), ImVec2(1, 0));

        ImGui::End();

        if (!showWindow) {
            tool->destroy();
            m_tools.erase(std::remove(m_tools.begin(), m_tools.end(), tool), m_tools.end());
        }
    }
#endif
}