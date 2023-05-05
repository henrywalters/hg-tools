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

void Tool::save() {
    if (m_saveFile == "") {
        throw std::runtime_error("No save file chosen, run saveAs first");
    }
    saveToDisc(m_saveFile);
}

void Tool::saveAs() {
    saveFile([&](auto file) {
        auto parts = hg::utils::f_getParts(file);
        m_saveFile = parts.path + (parts.extension == "" ? parts.name + m_fileExtension : parts.fullName);
        saveToDisc(m_saveFile);
    }, {m_fileExtension});
}

void Tool::load() {
    loadFile([&](auto file) {
        reset();
        m_saveFile = file;
        loadFromDisc(file);
    }, {m_fileExtension});
}

void Tool::renderMenu(double dt) {
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
