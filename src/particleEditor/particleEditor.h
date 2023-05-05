//
// Created by henry on 5/3/23.
//

#ifndef HGTOOLS_PARTICLEEDITOR_H
#define HGTOOLS_PARTICLEEDITOR_H

#include <hagame/graphics/tilemap.h>
#include <hagame/utils/spatialMap.h>
#include <hagame/core/assets.h>

#include "../tool.h"
#include "particleEmitter.h"

const std::string PARTICLE_EXT = ".hgp";
const std::string PARTICLE_PROJECT_EXT = ".hgp";

class ParticleEditor : public Tool {
public:
    ParticleEditor():
        Tool("Particle Editor", PARTICLE_PROJECT_EXT) {}

    hg::graphics::RawTexture<GL_RGBA>* getDisplay() override;
    void renderUI(double dt) override;

protected:

    void onInit() override;
    void onUpdate(double dt) override;
    void onDestroy() override;

    void reset() override;
    void saveToDisc(std::string file) override;
    void loadFromDisc(std::string file) override;

private:

    enum class RenderMode {
        Color,
    };

    float m_cameraPanSpeed = 500.0f;
    float m_cameraZoomSpeed = 100.0f;
    hg::graphics::OrthographicCamera m_camera;

    hg::graphics::RenderPasses<RenderMode> m_renderPasses;

    hg::graphics::Color m_color;

    std::string m_saveFile = "";

    std::unique_ptr<hg::graphics::primitives::Quad> m_quad;
    std::unique_ptr<hg::graphics::MeshInstance> m_mesh;

    std::unique_ptr<ParticleEmitter> m_particles;
};

#endif //HGTOOLS_PARTICLEEDITOR_H
