//
// Created by henry on 4/12/23.
//

#include <hagame/graphics/components/quad.h>
#include <hagame/core/assets.h>
#include <hagame/graphics/color.h>
#include <hagame/graphics/components/sprite.h>

#include "tilemapEditor.h"
#include "imgui.h"

using namespace hg;
using namespace hg::graphics;

void TilemapEditor::onInit() {
    //m_focus = m_entities.add();
    //m_focus->addComponent<Quad>(m_tileSize, m_tileSize * 0.5);

    m_focusQuad = std::make_unique<primitives::Quad>(m_tilemap.tileSize(), m_tilemap.tileSize() * 0.5);
    m_focus = std::make_unique<MeshInstance>(m_focusQuad.get());

    m_renderPasses.create(RenderMode::Color, FULL_HD);

    m_camera.size = FULL_HD;
    m_camera.centered = true;

    m_tilemap.addLayer();

    loadTexture("flag", "textures/flag.png");
}

void TilemapEditor::onUpdate(double dt) {

    m_renderPasses.clear(RenderMode::Color, Color::black());
    m_renderPasses.bind(RenderMode::Color);

    m_mousePos = m_camera.getGamePos(keyboardMouse.mouse.position);
    m_tileIdx = m_tilemap.getIndex(m_mousePos);

    if (keyboardMouse.mouse.left) {
        addTile(m_tileIdx);
    } else if (keyboardMouse.mouse.right) {
        removeTile();
    }

    m_camera.transform.position += keyboardMouse.lAxis.resize<3>() * m_cameraPanSpeed * dt;
    m_camera.zoom += keyboardMouse.mouse.wheel * m_cameraZoomSpeed * dt;

    auto shader = getShader("color");
    shader->use();
    shader->setMat4("model", Mat4::Translation(m_tilemap.getPos(m_tileIdx).resize<3>()));
    shader->setMat4("view", Mat4::Identity());
    shader->setMat4("projection", m_camera.projection());
    shader->setVec4("color", Color::blue());
    m_focus->render();

    shader = getShader("sprite");
    shader->use();
    shader->setMat4("view", Mat4::Identity());
    shader->setMat4("projection", m_camera.projection());

    m_tilemap.render(TileMode::Sprite, shader);

    m_tilemap.render(TileMode::SpawnPoint, shader, getTexture("flag"));

    shader = getShader("color");
    shader->use();
    shader->setMat4("view", Mat4::Identity());
    shader->setMat4("projection", m_camera.projection());

    m_tilemap.render(TileMode::Color, shader);

    m_renderPasses.render(RenderMode::Color, 1);

}

void TilemapEditor::onDestroy() {
    std::cout << "TILEMAP EDITOR Destroy\n";
}

RawTexture<GL_RGBA> *TilemapEditor::getDisplay() {
    return m_renderPasses.get(RenderMode::Color)->texture.get();
}

void TilemapEditor::renderUI(double dt) {

    int entityCount = 0;

    ImGui::Begin("Tilemap Editor Settings");
    ImGui::Text(("Entities: " + std::to_string(entityCount)).c_str());

    ImGui::Text(("Tile: " + m_tileIdx.toString()).c_str());

    for (int i = 0; i < TILE_MODE_NAMES.size(); i++) {
        if (ImGui::Button(TILE_MODE_NAMES[i].c_str())) {
            m_tileMode = (TileMode) i;
        }

        if (m_tileMode == (TileMode) i) {
            ImGui::SameLine();
            ImGui::Text("Active");
        }
    }

    ImGui::Text("Layers"); ImGui::SameLine();
    if (ImGui::Button("Add Layer")) {
        addLayer();
    }

    auto windowSize = ImGui::GetWindowSize();

    ImGui::Columns(3, "TableColumns", true);

    float col0Width = 50;
    float col2Width = 120;
    float col1Width = std::max<float>(windowSize.x - col0Width - col2Width, 50);

    ImGui::SetColumnWidth(0, col0Width);
    ImGui::SetColumnWidth(1, col1Width);
    ImGui::SetColumnWidth(2, col2Width);

    for (int i = 0; i < m_tilemap.layers(); i++) {

        if (i > 0) {
            ImGui::Separator();
        }

        bool visible = m_tilemap.getVisibility(i);

        ImGui::PushID(i);

        if (ImGui::Button(visible ? "Hide" : "Show")) {
            visible = !visible;
        }

        m_tilemap.setVisibility(i, visible);

        ImGui::NextColumn();

        ImGui::Text(("Layer " + std::to_string(i + 1) + "(" + std::to_string(m_tilemap.tileCount(i)) + " tiles)").c_str());
        ImGui::NextColumn();

        if (m_selectedLayer == i) {
            ImGui::Text("Active");
        } else if (ImGui::Button("Select Layer")) {
            m_selectedLayer = i;
        }
        ImGui::PopID();
        ImGui::NextColumn();
    }

    ImGui::Columns(1);

    ImGui::ColorPicker4("Tile color", m_color.vector);

    ImGui::Separator();

    if (ImGui::Button("Load Textures")) {
        loadFiles([&](std::vector<std::filesystem::path> files) {
            for (auto& file : files) {

                auto parts = utils::f_getParts(file);

                m_tilemap.addTexture(parts.name, file);

                if (m_selectedTexture == "") {
                    m_selectedTexture = parts.name;
                }
            }
            }, {".png"});
    }

    ImGui::Text("Textures");

    m_tilemap.textures()->forEach([&](auto key, auto texture) {
        if (ImGui::ImageButton(
                (void*)texture->id,
                ImVec2(50, 50),
                ImVec2(0, 0),
                ImVec2(1, 1),
                -1,
                ImColor(255, 255, 255, 255),
                m_selectedTexture == key ? ImColor(255, 255, 0, 255) : ImColor(255, 255, 255, 255)
            )) {
            m_selectedTexture = key;
        }
    });

    ImGui::End();
}

void TilemapEditor::addLayer() {
    m_tilemap.addLayer();
}

void TilemapEditor::addTile(hg::Vec2i idx) {

    if (m_tileMode == TileMode::Sprite && m_selectedTexture == "") {
        return;
    }

    Tile tile;
    tile.mode = m_tileMode;
    tile.color = m_color;


    if (m_tileMode == TileMode::Sprite) {
        tile.texture = m_selectedTexture;
    }

    m_tilemap.addTile(m_selectedLayer, m_tileIdx, tile);
}

void TilemapEditor::removeTile() {

    m_tilemap.removeTile(m_selectedLayer, m_tileIdx);
}

void TilemapEditor::reset() {
    m_tilemap.clear();
    m_selectedLayer = 0;
    m_selectedTexture = "";

    addLayer();
}

void TilemapEditor::saveToDisc(std::string file) {
    hg::utils::f_write(file, m_tilemap.save().toString());
}

void TilemapEditor::loadFromDisc(std::string file) {
    auto config = utils::MultiConfig::Parse(file);
    m_tilemap.load(config);
}


