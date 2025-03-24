//
// Created by socki on 3/19/2025.
//

#include "LevelEditor.h"

#include <fstream>
#include <string>
#include <nlohmann/json.hpp>

namespace Zayn {




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

    void ApplyEditorTheme(LevelEditor* editor)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        // Apply colors from the editor theme
//        colors[ImGuiCol_WindowBg]               = editor->theme.background;
//        colors[ImGuiCol_ChildBg]                = editor->theme.backgroundLight;
//        colors[ImGuiCol_PopupBg]                = editor->theme.backgroundDark;
//        colors[ImGuiCol_Border]                 = editor->theme.border;
//        colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
//        colors[ImGuiCol_FrameBg]                = editor->theme.backgroundLight;
//        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.25f, 0.25f, 0.27f, 1.00f);
//        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.30f, 0.30f, 0.33f, 1.00f);
//
//        // Headers & tabs
//        colors[ImGuiCol_Header]                 = editor->theme.accentDark;
//        colors[ImGuiCol_HeaderHovered]          = editor->theme.accent;
//        colors[ImGuiCol_HeaderActive]           = editor->theme.accentLight;
//        colors[ImGuiCol_Tab]                    = ImVec4(0.15f, 0.15f, 0.18f, 1.00f);
//        colors[ImGuiCol_TabHovered]             = editor->theme.accent;
//        colors[ImGuiCol_TabActive]              = editor->theme.accentDark;
//        colors[ImGuiCol_TabUnfocused]           = ImVec4(0.15f, 0.15f, 0.18f, 1.00f);
//        colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.20f, 0.20f, 0.25f, 1.00f);
//
//        // Title
//        colors[ImGuiCol_TitleBg]                = editor->theme.backgroundDark;
//        colors[ImGuiCol_TitleBgActive]          = editor->theme.accentDark;
//        colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.15f, 0.15f, 0.18f, 0.75f);
//
//        // Text
//        colors[ImGuiCol_Text]                   = editor->theme.text;
//        colors[ImGuiCol_TextSelectedBg]         = editor->theme.highlight;
//        colors[ImGuiCol_TextDisabled]           = editor->theme.textDimmed;

        // Buttons, checkboxes, etc.
        colors[ImGuiCol_Button]                 = ImVec4(0.25f, 0.25f, 0.27f, 1.00f);
//        colors[ImGuiCol_ButtonHovered]          = editor->theme.accent;
//        colors[ImGuiCol_ButtonActive]           = editor->theme.accentLight;
//        colors[ImGuiCol_CheckMark]              = editor->theme.accent;
//        colors[ImGuiCol_SliderGrab]             = editor->theme.accent;
//        colors[ImGuiCol_SliderGrabActive]       = editor->theme.accentLight;
//
//        // Accent colors
//        colors[ImGuiCol_ResizeGrip]             = ImVec4(0.20f, 0.30f, 0.50f, 0.30f);
//        colors[ImGuiCol_ResizeGripHovered]      = editor->theme.accent;
//        colors[ImGuiCol_ResizeGripActive]       = editor->theme.accentLight;
//        colors[ImGuiCol_SeparatorHovered]       = editor->theme.accent;
//        colors[ImGuiCol_SeparatorActive]        = editor->theme.accentLight;
//
//        // Scrollbar
//        colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.15f, 0.15f, 0.18f, 1.00f);
//        colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.25f, 0.25f, 0.30f, 1.00f);
//        colors[ImGuiCol_ScrollbarGrabHovered]   = editor->theme.accentDark;
//        colors[ImGuiCol_ScrollbarGrabActive]    = editor->theme.accent;

        // Style tweaks
//        style.WindowPadding = ImVec2(10, 10);
//        style.FramePadding = ImVec2(8, 4);
//        style.ItemSpacing = ImVec2(10, 8);
//        style.ItemInnerSpacing = ImVec2(6, 6);
//        style.TouchExtraPadding = ImVec2(0, 0);
//        style.IndentSpacing = 21;
//        style.ScrollbarSize = 14;
//        style.GrabMinSize = 10;
//
//        // Borders
//        style.WindowBorderSize = 1;
//        style.ChildBorderSize = 1;
//        style.PopupBorderSize = 1;
//        style.FrameBorderSize = 1;
//        style.TabBorderSize = 0;
//
//        // Rounding
//        style.WindowRounding = 6;
//        style.ChildRounding = 4;
//        style.FrameRounding = 4;
//        style.PopupRounding = 4;
//        style.ScrollbarRounding = 9;
//        style.GrabRounding = 4;
//        style.TabRounding = 4;
    }

    void InitLevelEditor(LevelEditor* levelEditor)
    {

        levelEditor->active = false;

        levelEditor->currentLevel.name = "Untitled";
        levelEditor->currentLevelPath = "";

        ApplyEditorTheme(levelEditor);


    }






} // Zayn

