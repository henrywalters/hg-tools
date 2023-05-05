//
// Created by henry on 4/12/23.
//

#ifndef HGAMETEMPLATE_TILEMAPEDITOR_H
#define HGAMETEMPLATE_TILEMAPEDITOR_H

#include <hagame/graphics/renderPasses.h>
#include <hagame/graphics/camera.h>
#include <hagame/graphics/tilemap.h>
#include <hagame/utils/spatialMap.h>
#include <hagame/core/assets.h>

#include "../tool.h"

const std::string TILEMAP_EXT = ".hgtm";
const std::string TILEMAP_PROJECT_EXT = ".hgtmp";

class TilemapEditor : public Tool {
public:
    TilemapEditor(hg::Vec2 tileSize = hg::Vec2(50, 50)):
        m_tilemap(tileSize),
        Tool("Tilemap Editor", TILEMAP_EXT) {}

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

    hg::Vec2 m_mousePos;
    hg::Vec2i m_tileIdx;

    hg::graphics::RenderPasses<RenderMode> m_renderPasses;

    std::unique_ptr<hg::graphics::primitives::Quad> m_focusQuad;
    std::unique_ptr<hg::graphics::MeshInstance> m_focus;

    std::string m_selectedTexture;

    int m_selectedLayer = 0;
    hg::graphics::TileMode m_tileMode = hg::graphics::TileMode::Color;

    hg::graphics::Color m_color;

    hg::graphics::Tilemap m_tilemap;

    void addLayer();
    void addTile(hg::Vec2i idx);
    void removeTile();

};

#endif //HGAMETEMPLATE_TILEMAPEDITOR_H
