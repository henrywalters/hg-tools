//
// Created by henry on 4/12/23.
//

#include <hagame/graphics/components/quad.h>
#include <hagame/core/assets.h>
#include <hagame/graphics/color.h>
#include <hagame/graphics/components/sprite.h>

#include "tilemapEditor.h"
#include "imgui.h"
#include "tile.h"

using namespace hg;
using namespace hg::graphics;

void TilemapEditor::onInit() {
    m_focus = m_entities.add();
    m_focus->addComponent<Quad>(m_tileSize, m_tileSize * 0.5);

    m_renderPasses.create(RenderMode::Color, FULL_HD);

    m_camera.size = FULL_HD;
    m_camera.centered = true;

    m_layers.push_back(hg::utils::SpatialMap2D<hg::Entity*, int>());
    m_layerVisible.push_back(true);
}

void TilemapEditor::onUpdate(double dt) {

    m_renderPasses.clear(RenderMode::Color, Color::black());
    m_renderPasses.bind(RenderMode::Color);

    m_mousePos = m_camera.getGamePos(keyboardMouse.mouse.position);
    m_tileIdx = (m_mousePos.div(m_tileSize)).floor().cast<int>();

    if (keyboardMouse.mouse.left) {
        addTile(m_tileIdx);
    } else if (keyboardMouse.mouse.right) {
        removeTile();
    }

    m_camera.transform.position += keyboardMouse.lAxis.resize<3>() * m_cameraPanSpeed * dt;
    m_camera.zoom += keyboardMouse.mouse.wheel * m_cameraZoomSpeed * dt;

    m_focus->transform.position = m_tileIdx.cast<float>().prod(m_tileSize).resize<3>();

    auto shader = getShader("color");
    shader->use();
    shader->setMat4("model", m_focus->transform.getModel());
    shader->setMat4("view", Mat4::Identity());
    shader->setMat4("projection", m_camera.projection());
    shader->setVec4("color", Color::blue());
    m_focus->getComponent<Quad>()->mesh()->render();

    shader = getShader("sprite");
    shader->use();
    shader->setMat4("view", Mat4::Identity());
    shader->setMat4("projection", m_camera.projection());


    m_entities.forEach<hg::graphics::Sprite>([&](hg::graphics::Sprite* sprite, hg::Entity* entity) {
        if (sprite->visible) {
            shader->setMat4("model", entity->transform.getModel());
            shader->setVec4("color", sprite->color);
            m_textures.get(sprite->texture)->bind();
            sprite->mesh()->render();
        }
    });

    shader = getShader("color");
    shader->use();
    shader->setMat4("view", Mat4::Identity());
    shader->setMat4("projection", m_camera.projection());

    m_entities.forEach<hg::graphics::Quad>([&](hg::graphics::Quad* quad, hg::Entity* entity) {
        shader->setMat4("model", entity->transform.getModel());
        shader->setVec4("color", quad->color);
        quad->mesh()->render();
    });

    m_renderPasses.render(RenderMode::Color, 1);

}

void TilemapEditor::onDestroy() {
    std::cout << "TILEMAP EDITOR Destroy\n";
}

hg::graphics::RawTexture<GL_RGBA> *TilemapEditor::getDisplay() {
    return m_renderPasses.get(RenderMode::Color)->texture.get();
}

void TilemapEditor::renderMenu(double dt) {
    ImGui::BeginMenuBar();
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("New")) {
            reset();
        }

        if (ImGui::MenuItem("Save As")) {
            saveAs();
        }

        if (m_saveFile != "" && ImGui::MenuItem("Save")) {
            save();
        }

        if (ImGui::MenuItem("Load")) {
            load();
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Edit"))
    {
        // Add items to the "Edit" menu here
        ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
}

void TilemapEditor::renderUI(double dt) {

    int entityCount = 0;
    m_entities.forEach([&](auto entity) {
        entityCount++;
    });


    ImGui::Begin("Tilemap Editor Settings");
    ImGui::Text(("Entities: " + std::to_string(entityCount)).c_str());

    ImGui::Text(("Tile: " + m_tileIdx.toString()).c_str());

    if (ImGui::Button("Color Tiles")) {
        m_tileMode = TileMode::Color;
    }

    if (m_tileMode == TileMode::Color) {
        ImGui::SameLine();
        ImGui::Text("Active");
    }

    if (ImGui::Button("Sprite Tiles")) {
        m_tileMode = TileMode::Sprite;
    }

    if (m_tileMode == TileMode::Sprite) {
        ImGui::SameLine();
        ImGui::Text("Active");
    }

    ImGui::Text("Layers"); ImGui::SameLine();
    if (ImGui::Button("Add Layer")) {
        addLayer();
    }

    auto windowSize = ImGui::GetWindowSize();

    ImGui::ColorPicker4("Tile color", m_color.vector);

    ImGui::Columns(3, "TableColumns", true);

    float col0Width = 50;
    float col2Width = 120;
    float col1Width = std::max<float>(windowSize.x - col0Width - col2Width, 50);

    ImGui::SetColumnWidth(0, col0Width);
    ImGui::SetColumnWidth(1, col1Width);
    ImGui::SetColumnWidth(2, col2Width);

    for (int i = 0; i < m_layers.size(); i++) {

        if (i > 0) {
            ImGui::Separator();
        }

        bool visible = m_layerVisible[i];

        ImGui::PushID(i);

        if (ImGui::Button(visible ? "Hide" : "Show")) {
            visible = !visible;
            m_entities.forEach<hg::graphics::Sprite>([&](hg::graphics::Sprite* sprite, hg::Entity* entity) {
                sprite->visible = visible;
            });
        }

        ImGui::NextColumn();

        ImGui::Text(("Layer " + std::to_string(i + 1) + "(" + std::to_string(m_layers[i].size()) + " tiles)").c_str());
        ImGui::NextColumn();

        if (m_selectedLayer == i) {
            ImGui::Text("Active");
        } else if (ImGui::Button("Select Layer")) {
            m_selectedLayer = i;
        }
        ImGui::PopID();
        ImGui::NextColumn();

        m_layerVisible[i] = visible;
    }

    ImGui::Columns(1);

    ImGui::Separator();

    if (ImGui::Button("Load Textures")) {
        loadFiles([&](std::vector<std::filesystem::path> files) {
            for (auto& file : files) {

                auto parts = utils::f_getParts(file);
                m_textures.set(parts.name, std::make_shared<Texture>(file));

                if (m_selectedTexture == "") {
                    m_selectedTexture = parts.name;
                }
            }
            }, {".png"});
    }

    ImGui::Text("Textures");

    m_textures.forEach([&](auto key, auto texture) {
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
    m_layers.push_back(hg::utils::SpatialMap2D<hg::Entity*, int>());
    m_layerVisible.push_back(true);
}

void TilemapEditor::addTile(hg::Vec2i idx) {

    if (m_tileMode == TileMode::Sprite && m_selectedTexture == "") {
        return;
    }

    auto pos = idx.cast<float>().prod(m_tileSize).resize<3>();

    if (m_layers[m_selectedLayer].has(idx)) {
        return;
    }

    auto entity = m_entities.add();
    entity->transform.position = pos;
    entity->transform.position[2] = m_selectedLayer;

    auto tile = entity->addComponent<Tile>();
    tile->layer = m_selectedLayer;

    if (m_tileMode == TileMode::Color) {
        auto quad = entity->addComponent<hg::graphics::Quad>(m_tileSize, m_tileSize * 0.5);
        quad->color = m_color;
    } else if (m_tileMode == TileMode::Sprite) {
        auto sprite = entity->addComponent<hg::graphics::Sprite>(m_tileSize, m_tileSize * 0.5);
        sprite->texture = m_selectedTexture;
    }

    m_layers[m_selectedLayer].insert(idx, entity);
}

void TilemapEditor::removeTile() {

    if (m_layers[m_selectedLayer].has(m_tileIdx)) {
        for (const auto entity : m_layers[m_selectedLayer].get(m_tileIdx)) {
            m_entities.remove(entity);
        }
        m_layers[m_selectedLayer].clear(m_tileIdx);
    }
}

void TilemapEditor::reset() {
    m_entities.clear();
    m_textures.clear();
    m_layers.clear();
    m_selectedLayer = 0;
    m_selectedTexture = "";

    m_focus = m_entities.add();
    m_focus->addComponent<Quad>(m_tileSize, m_tileSize * 0.5);

    addLayer();
}

void TilemapEditor::save() {
    hg::utils::MultiConfig config;

    config.addPage("project");
    auto project = config.getPage("project");

    project->addSection("meta");
    project->set("meta", "layer_count", m_layers.size());

    config.addPage("textures");
    auto textures = config.getPage("textures");

    m_textures.forEach([&](auto key, auto texture) {
        textures->addSection(key);
        textures->setRaw(key, "path", texture->image->path);
    });

    config.addPage("tiles");
    auto tiles = config.getPage("tiles");

    m_entities.forEach<Tile>([&](auto tile, auto entity) {
        auto id = std::to_string(entity->id());
        tiles->addSection(id);
        tiles->setArray<float, 3>(id, "position", entity->transform.position.vector);
        tiles->set(id, "layer", tile->layer);
        tiles->set(id, "mode", (int) tile->mode);

        if (tile->mode == TileMode::Sprite) {
            auto sprite = entity->template getComponent<Sprite>();
            tiles->setArray<float, 4>(id, "color", sprite->color.vector);
            tiles->setRaw(id, "texture", sprite->texture);
        } else if (tile->mode == TileMode::Color) {
            auto quad = entity->template getComponent<Quad>();
            tiles->setArray<float, 4>(id, "color", quad->color.vector);
        }
    });

    hg::utils::f_write(m_saveFile, config.toString());
}

void TilemapEditor::saveAs() {
    saveFile([&](auto file){
        auto parts = hg::utils::f_getParts(file);
        m_saveFile = parts.path + (parts.extension == "" ? parts.name + TILEMAP_PROJECT_EXT : parts.fullName);
        save();
    }, {TILEMAP_PROJECT_EXT});
}

void TilemapEditor::load() {
    loadFile([&](auto file) {
        reset();
        utils::MultiConfig config = utils::MultiConfig::Parse(file);

        auto project = config.getPage("project");
        auto textures = config.getPage("textures");
        auto tiles = config.getPage("tiles");

        int layerCount = project->get<int>("meta", "layer_count");

        for (int i = 0; i < layerCount; i++) {
            addLayer();
        }

        for (const auto& texture : textures->sections()) {
            m_textures.set(texture, std::make_shared<Texture>(textures->getRaw(texture, "path")));
        }

        for (const auto& id : tiles->sections()) {
            tiles->getArray<float, 4>(id, "color", m_color.vector);
            m_selectedLayer = tiles->get<int>(id, "layer");
            m_tileMode = (TileMode) tiles->get<int>(id, "mode");
            if (m_tileMode == TileMode::Sprite) {
                m_selectedTexture = tiles->getRaw(id, "texture");
            }
            Vec3 pos;
            tiles->getArray<float, 3>(id, "position", pos.vector);
            auto tileIdx = pos.resize<2>().div(m_tileSize).cast<int>();
            addTile(tileIdx);
        }

    });
}



