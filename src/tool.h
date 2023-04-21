//
// Created by henry on 4/13/23.
//

#ifndef HGTOOLS_TOOL_H
#define HGTOOLS_TOOL_H

#include <hagame/core/game.h>
#include <hagame/graphics/rawTexture.h>
#include <hagame/input/devices/keyboardMouse.h>
#include <hagame/utils/file.h>

#include "imgui.h"
#include "imfilebrowser.h"

class Tool : public hg::Game {
public:

    Tool(std::string name):
        hg::Game(name)
    {}

    void render(double dt);
    virtual void renderMenu(double dt) {}

    virtual hg::graphics::RawTexture<GL_RGBA>* getDisplay() = 0;

    hg::input::devices::KeyboardMouse keyboardMouse;

protected:

    void loadFile(std::function<void(std::filesystem::path)> onChoose, std::vector<std::string> filters = {});
    void loadFiles(std::function<void(std::vector<std::filesystem::path>)> onChoose, std::vector<std::string> filters = {});
    void saveFile(std::function<void(std::filesystem::path)> onChoose, std::vector<std::string> filters = {});


    virtual void renderUI(double dt) {}

private:

    std::unique_ptr<ImGui::FileBrowser> m_files;
    bool m_browsingFiles = false;
    bool m_choosingMany = false;
    std::function<void(std::filesystem::path)> m_onChooseOne;
    std::function<void(std::vector<std::filesystem::path>)> m_onChooseMany;

};

#endif //HGTOOLS_TOOL_H
