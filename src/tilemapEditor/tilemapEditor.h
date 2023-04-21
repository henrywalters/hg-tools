//
// Created by henry on 4/12/23.
//

#ifndef HGAMETEMPLATE_TILEMAPEDITOR_H
#define HGAMETEMPLATE_TILEMAPEDITOR_H

#include <hagame/graphics/renderPasses.h>
#include <hagame/graphics/camera.h>
#include <hagame/utils/spatialMap.h>

#include "../tool.h"
#include "tile.h"

const std::string TILEMAP_EXT = ".hgtm";
const std::string TILEMAP_PROJECT_EXT = ".hgtmp";

enum class RenderMode {
    Color,
};

class TilemapEditor : public Tool {
public:
    TilemapEditor(hg::Vec2 tileSize = hg::Vec2(50, 50)):
        m_tileSize(tileSize),
        Tool("Tilemap Editor")
        {}

    hg::graphics::RawTexture<GL_RGBA>* getDisplay() override;
    void renderUI(double dt) override;
    void renderMenu(double dt) override;

protected:

    void onInit() override;
    void onUpdate(double dt) override;
    void onDestroy() override;

private:

    float m_cameraPanSpeed = 500.0f;
    float m_cameraZoomSpeed = 100.0f;
    hg::graphics::OrthographicCamera m_camera;

    hg::Vec2 m_mousePos;
    hg::Vec2i m_tileIdx;

    hg::graphics::RenderPasses<RenderMode> m_renderPasses;

    hg::Vec2 m_tileSize;

    hg::EntityManager m_entities;
    hg::Entity* m_focus;

    hg::utils::Store<std::shared_ptr<hg::graphics::Texture>> m_textures;

    std::string m_selectedTexture;

    int m_selectedLayer = 0;
    TileMode m_tileMode;

    std::vector<hg::utils::SpatialMap2D<hg::Entity*, int>> m_layers;
    std::vector<bool> m_layerVisible;

    hg::graphics::Color m_color;

    std::string m_saveFile = "";

    void addLayer();
    void addTile(hg::Vec2i idx);
    void removeTile();

    void reset();
    void save();
    void saveAs();
    void load();

};

#endif //HGAMETEMPLATE_TILEMAPEDITOR_H
