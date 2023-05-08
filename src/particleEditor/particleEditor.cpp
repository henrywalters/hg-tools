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
    int bufferSize = m_particles->settings.requiredBufferSize();
    ImGui::Text(("FPS: " + std::to_string(1.0 / dt)).c_str());
    ImGui::Text(("Buffer size: " + std::to_string(bufferSize)).c_str());
    ImGui::ColorPicker4("Start Color", m_particles->settings.startColor.vector);
    ImGui::ColorPicker4("End Color", m_particles->settings.endColor.vector);
    ImGui::Checkbox("Position Relative", &m_particles->settings.positionRelative);
    ImGui::DragInt("Particles per Second", &m_particles->settings.particlesPerSecond, 1, 1, 10000);
    ImGui::DragFloat("Min angle", &m_particles->settings.angle.lower, 1.0, 0, 360);
    ImGui::DragFloat("Max angle", &m_particles->settings.angle.upper, 1.0, 0, 360);
    ImGui::DragFloat("Min Speed", &m_particles->settings.speed.lower, 1.0, 0, 10000);
    ImGui::DragFloat("Max Speed", &m_particles->settings.speed.upper, 1.0, 0, 10000);
    ImGui::DragFloat("Min Alive For", &m_particles->settings.aliveFor.lower, 0.01, 0, 5);
    ImGui::DragFloat("Max Alive For", &m_particles->settings.aliveFor.upper, 0.01, 0, 5);
    ImGui::DragFloat3("Gravity", m_particles->settings.gravity.vector, 0.01, -1000, 1000);
}

void ParticleEditor::onInit() {
    m_renderPasses.create(RenderMode::Color, FULL_HD);

    m_camera.size = FULL_HD;
    m_camera.centered = true;

    m_quad = std::make_unique<primitives::Quad>(Vec2(5, 5), Vec2(-0.5, -0.5));
    m_mesh = std::make_unique<MeshInstance>(m_quad.get());

    m_particles = std::make_unique<ParticleEmitter>(10000, m_mesh.get());
}

void ParticleEditor::onUpdate(double dt) {

    auto mousePos = m_camera.getGamePos(keyboardMouse.mouse.position);

    m_renderPasses.clear(RenderMode::Color, Color::black());
    m_renderPasses.bind(RenderMode::Color);

    m_particles->update(mousePos.resize<3>(), dt);

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
    m_particles->settings = ParticleEmitterSettings();
}

void ParticleEditor::saveToDisc(std::string file) {
    hg::utils::f_write(file, m_particles->settings.save().toString());
}

void ParticleEditor::loadFromDisc(std::string file) {
    m_particles->settings.load(hg::utils::Config::Parse(hg::utils::f_readLines(file)));
}
