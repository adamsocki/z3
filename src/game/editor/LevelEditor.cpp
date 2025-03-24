//
// Created by socki on 3/19/2025.
//

#include "LevelEditor.h"

#include <fstream>
#include <string>
#include <nlohmann/json.hpp>

namespace Zayn {

    std::vector<std::string> GetLevelFiles(const std::string& folderPath) {
        std::vector<std::string> levelFiles;

        try {
            for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
                if (entry.is_regular_file()) {
                    std::string path = entry.path().string();
                    // Check if file has the .zlvl extension
                    if (path.size() >= 5 && path.substr(path.size() - 5) == ".zlvl") {
                        levelFiles.push_back(path);
                    }
                }
            }
        } catch (const std::exception& e) {
            // Handle exceptions (e.g., directory doesn't exist)
            printf("Error reading directory: %s\n", e.what());
        }

        return levelFiles;
    }

    void ClearLevelData(LevelData* levelData) {
        // Reset metadata
        levelData->name = "";
        levelData->version = 0;
        levelData->fileName = "";
        // memset(levelData->fileName, 0, sizeof(levelData->name));
        //levelData->ambientLight = V3(0.2f, 0.2f, 0.2f);  // Default ambient light
        //memset(levelData->skyboxTexturePath, 0, sizeof(levelData->skyboxTexturePath));

        // Clear entities, but don't free memory; just reset the count
        DynamicArrayClear(&levelData->levelEntityHandles);
    }


    void CreateNewLevel(CreateNewLevelInfo info) {

    }

    bool SaveLevel(LevelData levelData)
    {
        nlohmann::json levelJson;

        levelJson["name"]    = levelData.name;
        levelJson["version"] = levelData.version;

        // write json to file
        try {
            std::string fullPath = std::string("../src/game/levels/") + levelData.name;
            if (!fullPath.ends_with(".zlvl")) {
                fullPath += ".zlvl";
            }

            std::filesystem::create_directories("levels");
            std::ofstream file(fullPath);
            if (!file.is_open()) {
                printf("Failed to open file for writing: %s\n", fullPath.c_str());
                return false;
            }

            file << levelJson.dump(4);  // Pretty print with 4-space indentation
            file.close();

            printf("Level saved successfully to: %s\n", fullPath.c_str());
            return true;

        } catch (const std::exception& e) {
            printf("Error saving level: %s\n", e.what());
            return false;
        }

    }

    bool LoadLevel(LevelData* currentLevelData, std::string levelFile) {
        // Read from JSON
        // Apply to LevelEditor

        ClearLevelData(currentLevelData);

        try {
            std::string fullPath =  levelFile;
            if (!fullPath.ends_with(".zlvl")) {
                fullPath += ".zlvl";
            }
            std::ifstream file(fullPath);
            if (!file.is_open()) {
                printf("Failed to open level file: %s\n", fullPath.c_str());
                return false;
            }

            nlohmann::json levelJson;
            file >> levelJson;
            file.close();

            int version = levelJson["version"];
            if (version > 1) {
                printf("Warning: Level file version (%d) is newer than supported version (1)\n", version);
            }

            currentLevelData->name = levelJson["name"].get<std::string>();
            currentLevelData->version = levelJson["version"];


            printf("Level loaded successfully from: %s\n", fullPath.c_str());
            return true;
        } catch (const std::exception& e) {
            printf("Error loading level: %s\n", e.what());
            return false;
        }

    }

    void ApplyEditorTheme(LevelEditor* editor)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        colors[ImGuiCol_Button]                 = ImVec4(0.25f, 0.25f, 0.27f, 1.00f);

    }

    void InitLevelEditor(LevelEditor* levelEditor)
    {

        levelEditor->active = false;

        levelEditor->currentLevelData.name = "Untitled";
        levelEditor->currentLevelPath = "";

        ApplyEditorTheme(levelEditor);


    }






} // Zayn

