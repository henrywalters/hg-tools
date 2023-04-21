//
// Created by henry on 4/14/23.
//

#include "tool.h"

void Tool::render(double dt) {
    if (m_browsingFiles) {
        m_files->Display();
        if (m_files->HasSelected()) {
            if (m_choosingMany) {
                m_onChooseMany(m_files->GetMultiSelected());
            } else {
                m_onChooseOne(m_files->GetSelected());
            }

            m_browsingFiles = false;
            m_files.reset();
        }
    }
    renderUI(dt);
}

void Tool::loadFile(std::function<void(std::filesystem::path)> onChoose, std::vector<std::string> filters) {
    if (m_browsingFiles) {
        return;
    }

    m_files = std::make_unique<ImGui::FileBrowser>();
    m_files->SetTitle("Load file");
    m_files->SetTypeFilters(filters);

    m_browsingFiles = true;
    m_onChooseOne = onChoose;
    m_choosingMany = false;

    m_files->Open();
}

void
Tool::loadFiles(std::function<void(std::vector<std::filesystem::path>)> onChoose, std::vector<std::string> filters) {
    if (m_browsingFiles) {
        return;
    }

    m_files = std::make_unique<ImGui::FileBrowser>(ImGuiFileBrowserFlags_MultipleSelection);
    m_files->SetTitle("Load file");
    m_files->SetTypeFilters(filters);

    m_browsingFiles = true;
    m_onChooseMany = onChoose;
    m_choosingMany = true;

    m_files->Open();
}

void Tool::saveFile(std::function<void(std::filesystem::path)> onChoose, std::vector<std::string> filters) {
    if (m_browsingFiles) {
        return;
    }

    m_files = std::make_unique<ImGui::FileBrowser>(ImGuiFileBrowserFlags_EnterNewFilename);
    m_files->SetTitle("Save file");
    m_files->SetTypeFilters(filters);

    m_browsingFiles = true;
    m_onChooseOne = onChoose;
    m_choosingMany = false;

    m_files->Open();
}
