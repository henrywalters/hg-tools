//
// Created by henry on 5/3/23.
//

#ifndef HGTOOLS_PARTICLEEMITTER_H
#define HGTOOLS_PARTICLEEMITTER_H

#include <hagame/graphics/buffer.h>
#include <hagame/graphics/mesh.h>
#include <hagame/utils/clock.h>
#include <hagame/utils/random.h>
#include <hagame/math/interval.h>
#include <hagame/graphics/shaderProgram.h>

#include "particle.h"

struct ParticleEmitterSettings {
    float radius = M_PI * 2.0f;
    hg::math::Interval<float> speed = hg::math::Interval<float>(100, 500);
    hg::math::Interval<float> aliveFor = hg::math::Interval<float>(0.5, 2.0);
    hg::graphics::Color startColor = hg::graphics::Color::red();
    hg::graphics::Color endColor = hg::graphics::Color::red();
};

class ParticleEmitter {
public:

    ParticleEmitter(size_t maxParticles, hg::graphics::MeshInstance* mesh);

    ParticleEmitterSettings settings;

    void emit(hg::Vec3 pos);
    void clear();
    void render(hg::graphics::ShaderProgram* shader);

private:

    size_t m_maxParticles;
    hg::graphics::MeshInstance* m_mesh;
    std::unique_ptr<hg::graphics::VertexBuffer<Particle>> m_buffer;

    int m_queueStart = 0;
    int m_queueEnd = 0;

    long long m_startTime;
};

#endif //HGTOOLS_PARTICLEEMITTER_H
