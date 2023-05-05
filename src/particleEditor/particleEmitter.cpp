//
// Created by henry on 5/3/23.
//
#include "particleEmitter.h"

using namespace hg;
using namespace hg::graphics;

ParticleEmitter::ParticleEmitter(size_t maxParticles, MeshInstance* mesh):
    m_maxParticles(maxParticles),
    m_mesh(mesh),
    m_buffer(VertexBuffer<Particle>::Dynamic(m_maxParticles)),
    m_startTime(utils::Clock::Now())
{
    auto vao = m_mesh->getVAO();
    m_buffer->bind();
    vao->bind();
    vao->defineAttribute(m_buffer.get(), DataType::Float, 3, 3, offsetof(Particle, startPos));
    vao->defineAttribute(m_buffer.get(), DataType::Float, 4, 3, offsetof(Particle, velocity));
    vao->defineAttribute(m_buffer.get(), DataType::Float, 5, 4, offsetof(Particle, color));
    vao->defineAttribute(m_buffer.get(), DataType::Float, 6, 1, offsetof(Particle, startTime));
    vao->defineAttribute(m_buffer.get(), DataType::Float, 7, 1, offsetof(Particle, aliveFor));
    vao->setInstanced(3, 6);

}

void ParticleEmitter::emit(hg::Vec3 pos) {
    utils::Random rand;
    Particle particle;
    particle.velocity = Vec3(rand.real<float>(-1, 1), rand.real<float>(-1, 1), 0).normalized() * 100;
    particle.color = Color::red();
    particle.startTime = utils::Clock::ToSeconds(utils::Clock::Now() - m_startTime);
    particle.startPos = pos + Vec3(0, 0, 0);
    particle.aliveFor = 1;
    m_buffer->update(m_queueEnd++, particle);

    m_queueEnd = m_queueEnd % m_maxParticles;
}

void ParticleEmitter::render(hg::graphics::ShaderProgram* shader) {
    shader->use();
    m_mesh->getVAO()->bind();
    shader->setFloat("currentTime", utils::Clock::ToSeconds(utils::Clock::Now() - m_startTime));
    glDrawArraysInstanced(GL_TRIANGLES, 0, m_mesh->size(), m_maxParticles);
}
