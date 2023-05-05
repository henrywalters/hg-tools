//
// Created by henry on 5/3/23.
//

#ifndef HGTOOLS_PARTICLE_H
#define HGTOOLS_PARTICLE_H

#include <hagame/math/aliases.h>
#include <hagame/graphics/color.h>

struct Particle {
    hg::Vec3 startPos;
    hg::Vec3 velocity;
    hg::graphics::Color color;
    float startTime;
    float aliveFor;
};



#endif //HGTOOLS_PARTICLE_H
