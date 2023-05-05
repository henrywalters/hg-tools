//
// Created by henry on 5/3/23.
//
#include "particleEditor.h"
#include <hagame/graphics/resolution.h>

using namespace hg;
using namespace hg::graphics;

hg::graphics::RawTexture<GL_RGBA> *ParticleEditor::getDisplay() {
    return m_renderPasses.get(RenderMode::Color)->texture.get();
}

void ParticleEditor::renderUI(double dt) {
    ImGui::Text(std::to_string(dt).c_str());
}

void ParticleEditor::onInit() {
    m_renderPasses.create(RenderMode::Color, FULL_HD);

    m_camera.size = FULL_HD;
    m_camera.centered = true;

    m_quad = std::make_unique<primitives::Quad>(Vec2(5, 5), Vec2(-0.5, -0.5));
    m_mesh = std::make_unique<MeshInstance>(m_quad.get());

    m_particles = std::make_unique<ParticleEmitter>(1000000, m_mesh.get());
}

void ParticleEditor::onUpdate(double dt) {

    auto mousePos = m_camera.getGamePos(keyboardMouse.mouse.position);

    m_renderPasses.clear(RenderMode::Color, Color::black());
    m_renderPasses.bind(RenderMode::Color);

    for (int i = 0; i < 100; i++) {
        m_particles->emit(mousePos.resize<3>());
    }

    auto shader = getShader("particle");
    shader->use();
    shader->setMat4("model", Mat4::Identity());
    shader->setMat4("projection", m_camera.projection());
    shader->setMat4("view", m_camera.view());

    m_particles->render(shader);

    m_renderPasses.render(RenderMode::Color, 1);
}

void ParticleEditor::onDestroy() {
    Game::onDestroy();
}

void ParticleEditor::reset() {

}

void ParticleEditor::saveToDisc(std::string file) {
    Tool::saveToDisc(file);
}

void ParticleEditor::loadFromDisc(std::string file) {
    Tool::loadFromDisc(file);
}
