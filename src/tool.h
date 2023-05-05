//
// Created by henry on 4/13/23.
//

#ifndef HGTOOLS_TOOL_H
#define HGTOOLS_TOOL_H

#include <hagame/core/game.h>
#include <hagame/graphics/renderPasses.h>
#include <hagame/graphics/camera.h>
#include <hagame/graphics/rawTexture.h>
#include <hagame/input/devices/keyboardMouse.h>
#include <hagame/utils/file.h>

#include "imgui.h"
#include "imfilebrowser.h"

class Tool : public hg::Game {
public:

    Tool(std::string name, std::string fileExtension):
        hg::Game(name),
        m_fileExtension(fileExtension)
    {}

    void render(double dt);
    void renderMenu(double dt);

    virtual hg::graphics::RawTexture<GL_RGBA>* getDisplay() = 0;

    hg::input::devices::KeyboardMouse keyboardMouse;

protected:

    virtual void reset() {};
    virtual void saveToDisc(std::string saveFile) {};
    virtual void loadFromDisc(std::string saveFile) {};

    void loadFile(std::function<void(std::filesystem::path)> onChoose, std::vector<std::string> filters = {});
    void loadFiles(std::function<void(std::vector<std::filesystem::path>)> onChoose, std::vector<std::string> filters = {});
    void saveFile(std::function<void(std::filesystem::path)> onChoose, std::vector<std::string> filters = {});

    void save();
    void saveAs();
    void load();

    virtual void renderUI(double dt) {}

private:

    std::unique_ptr<ImGui::FileBrowser> m_files;
    bool m_browsingFiles = false;
    bool m_choosingMany = false;
    std::function<void(std::filesystem::path)> m_onChooseOne;
    std::function<void(std::vector<std::filesystem::path>)> m_onChooseMany;

    std::string m_saveFile = "";
    std::string m_fileExtension = "";

};

#endif //HGTOOLS_TOOL_H
