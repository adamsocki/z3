//
// Created by socki on 3/19/2025.
//

#include "myImgui_LevelEditor.h"
#include "imgui.h"
#include <imgui_internal.h>
#include "../entities/Entity.h"

namespace Zayn {

    // Scene hierarchy window
    void RenderSceneHierarchyWindow(LevelEditor* levelEditor, EntityFactory* entityFactory, Engine* engine) {

        ImGui::Begin("Scene Hierarchy", &levelEditor->hierarchyWindowOpen);
        // Search bar for filtering entities
        static char searchBuffer[256] = "";
        ImGui::InputText("Search", searchBuffer, sizeof(searchBuffer));
        ImGui::Separator();

        // Entity list
//        for (int i = 0; i < levelEditor->entityCount; i++) {
            for (int i = 0; i < 1; i++) {

            //EntityHandle entityHandle = levelEditor->currentLevel.levelEntityHandles[i];

//            Game::PlayerEntity* playerEntity = (Game::PlayerEntity*)GetEntity(entityFactory, engine->HTEST);

            Game::Entity* entity = static_cast<Game::Entity*>(GetEntity(entityFactory, engine->HTEST));


            static char nameBuffer[256];
            strncpy(nameBuffer, entity->name.c_str(), sizeof(nameBuffer) - 1);
            if (ImGui::InputText("Entity Name", nameBuffer, sizeof(nameBuffer))){

            }


//            Entity*


            // Skip if doesn't match search
//            if (searchBuffer[0] != '\0' && entity.name.find(searchBuffer) == std::string::npos) {
//                continue;
//            }
//
//            // Check if this entity is selected
//            bool isSelected = levelEditor->hasSelection &&
//                              entity.handle.indexInInfo == levelEditor->selectedEntity.indexInInfo &&
//                              entity.handle.generation == levelEditor->selectedEntity.generation &&
//                              entity.handle.type == levelEditor->selectedEntity.type;
//
//            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
//            if (isSelected) flags |= ImGuiTreeNodeFlags_Selected;
//
//            ImGui::TreeNodeEx(entity.name.c_str(), flags);
//
//            // Handle selection
//            if (ImGui::IsItemClicked()) {
//                SelectEntity(levelEditor, entity.handle);
//            }
//
//            // Context menu
//            if (ImGui::BeginPopupContextItem()) {
//                if (ImGui::MenuItem("Rename")) {
//                    // TODO: Implement rename
//                }
//                if (ImGui::MenuItem("Duplicate")) {
//                    // TODO: Implement duplicate
//                }
//                if (ImGui::MenuItem("Delete")) {
//                    if (isSelected) {
//                       // DeleteSelectedEntity(editor);
//                    } else {
//                        // TODO: Implement delete for non-selected entity
//                    }
//                }
//                ImGui::EndPopup();
//            }
        }

        // Context menu for background (to create new entities)
//        if (ImGui::BeginPopupContextWindow(NULL, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
//            if (ImGui::MenuItem("Create Empty Entity")) {
//                //CreateEntityInEditor(editor, Game::EntityType_Player, "New Entity");
//            }
//            ImGui::EndPopup();
//        }

        ImGui::End();


    }

    void RenderAssetGrid(LevelEditor* editor, const char* assetNames[], int assetCount) {
        // Calculate columns based on window width
        float windowWidth = ImGui::GetContentRegionAvail().x;
        float thumbnailSize = 80.0f;
        int columns = Max(1, (int)(windowWidth / (thumbnailSize + 10)));

//        if (ImGui::BeginTable("AssetGrid", columns)) {
//            for (int i = 0; i < assetCount; i++) {
//                ImGui::TableNextColumn();
//
//                // Asset thumbnail/icon
//                ImGui::Button(assetNames[i], ImVec2(thumbnailSize, thumbnailSize));
//
//                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
//                    // Double-click to use asset
//                    // TODO: Handle double-click based on asset type
//                }
//
//                // Drag and drop source
//                if (ImGui::BeginDragDropSource()) {
//                    ImGui::SetDragDropPayload("ASSET", assetNames[i], strlen(assetNames[i]) + 1);
//                    ImGui::Text("Asset: %s", assetNames[i]);
//                    ImGui::EndDragDropSource();
//                }
//
//                // Context menu
//                if (ImGui::BeginPopupContextItem()) {
//                    if (ImGui::MenuItem("Import")) { /* TODO: Import asset */ }
//                    if (ImGui::MenuItem("Delete")) { /* TODO: Delete asset */ }
//                    if (ImGui::MenuItem("Rename")) { /* TODO: Rename asset */ }
//                    ImGui::EndPopup();
//                }
//
//                // Asset name label
//                ImGui::TextWrapped("%s", assetNames[i]);
//            }
//            ImGui::EndTable();
//        }
    }


    void MyRenderLevelEditorWindow(LevelEditor* levelEditor, EntityFactory* entityFactory, Engine* engine)
    {
        if (ImGui::BeginMainMenuBar()) {

            if (ImGui::BeginMenu("View")) {
                if (ImGui::MenuItem("Scene Hierarchy", NULL, &levelEditor->hierarchyWindowOpen)) {}
                if (ImGui::MenuItem("Inspector", NULL, &levelEditor->inspectorWindowOpen)) {}
                if (ImGui::MenuItem("Asset Browser", NULL, &levelEditor->assetBrowserWindowOpen)) {}
                if (ImGui::MenuItem("Settings", NULL, &levelEditor->settingsWindowOpen)) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Grid", NULL, &levelEditor->viewport.showGrid)) {}
                if (ImGui::MenuItem("Wireframe Mode", NULL, &levelEditor->viewport.wireframeMode)) {}
                ImGui::EndMenu();
            }


            ImGui::EndMainMenuBar();
        }



        ImGui::Begin("Level Editor");


        if (ImGui::CollapsingHeader("Level Controls", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::Button("New Level..."))
            {

            }
            if (ImGui::Button("Open Level..."))
            {

            }
            ImGui::Separator();
            if (ImGui::Button("Save Level..."))
            {

            }
            if (ImGui::Button("Save Level As..."))
            {

            }
        }


        if (ImGui::CollapsingHeader("Create")) {

            if(ImGui::TreeNode("Create 3D") )
            {

                ImGui::TreePop();
                ImGui::Spacing();
            }
            if(ImGui::TreeNode("Create Entity") )
            {

                ImGui::TreePop();
                ImGui::Spacing();
            }
            if(ImGui::TreeNode("Create Lighting") )
            {
                if (ImGui::Button(" Directional Light")) {
                    // TODO: Create directional light
                }
                if (ImGui::Button(" Point Light")) {
                    // TODO: Create point light
                }
                if (ImGui::Button(" Spot Light")) {
                    // TODO: Create spot light
                }
                ImGui::TreePop();
                ImGui::Spacing();
            }


        }
        ImGui::End();

        // Asset Grid
        RenderSceneHierarchyWindow(levelEditor,&engine->entityFactory, engine);



        // Settings window
        ImGui::Begin("Settings", &levelEditor->settingsWindowOpen);
        if (ImGui::CollapsingHeader("Grid Settings")) {
            ImGui::Checkbox("Show Grid", &levelEditor->viewport.showGrid);
            ImGui::SliderFloat("Cell Size", &levelEditor->grid.cellSize, 0.1f, 10.0f);
            ImGui::SliderInt("Major Line Interval", &levelEditor->grid.majorLineInterval, 1, 20);
            ImGui::ColorEdit4("Minor Line Color", &levelEditor->grid.minorLineColor.x);
            ImGui::ColorEdit4("Major Line Color", &levelEditor->grid.majorLineColor.x);
            ImGui::SliderInt("Grid Size", &levelEditor->grid.size, 10, 1000);
        }
        if (ImGui::CollapsingHeader("Camera Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::SliderFloat("Movement Speed", &levelEditor->camera.currentSpeed, 1.0f, 100.0f);
            ImGui::SliderFloat("Rotation Speed", &levelEditor->camera.rotationSpeed, 0.001f, 0.02f);
            ImGui::SliderFloat("Field of View", &levelEditor->camera.vFOV, 30.0f, 120.0f);
            ImGui::SliderFloat("Near Plane", &levelEditor->camera.nearPlane, 0.01f, 10.0f);
            ImGui::SliderFloat("Far Plane", &levelEditor->camera.farPlane, 100.0f, 10000.0f);

            if (ImGui::Button("Reset Camera")) {
                levelEditor->camera.position = V3(0.0f, 10.0f, 10.0f);
                levelEditor->camera.targetPos = V3(0.0f, 0.0f, 0.0f);
                levelEditor->camera.up = V3(0.0f, 1.0f, 0.0f);
            }
        }
        if (ImGui::CollapsingHeader("Manipulation Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
            const char* gizmoModes[] = { "Translate", "Rotate", "Scale" };
            int currentGizmoMode = (int)levelEditor->gizmoMode;
            if (ImGui::Combo("Gizmo Mode", &currentGizmoMode, gizmoModes, IM_ARRAYSIZE(gizmoModes))) {
                levelEditor->gizmoMode = (GizmoMode)currentGizmoMode;
            }

            const char* snapModes[] = { "No Snap", "Grid Snap", "Vertex Snap" };
            int currentSnapMode = (int)levelEditor->snapMode;
            if (ImGui::Combo("Snap Mode", &currentSnapMode, snapModes, IM_ARRAYSIZE(snapModes))) {
                levelEditor->snapMode = (SnapMode)currentSnapMode;
            }

            ImGui::SliderFloat("Snap Value", &levelEditor->snapValue, 0.1f, 10.0f);
        }

        ImGui::End();


        ImGui::Begin("Scene Hierarchy", &levelEditor->hierarchyWindowOpen);

        // Search bar for filtering entities
        static char searchBuffer[256] = "";
        ImGui::InputText("Search", searchBuffer, sizeof(searchBuffer));
        ImGui::Separator();

        ImGui::End();


        ImGui::Begin("Asset Browser", &levelEditor->assetBrowserWindowOpen);

        ImGui::Text("Current path: %s", levelEditor->currentAssetDirectory.c_str());
        if (ImGui::Button("Up")) {
            // TODO: Navigate up in folder hierarchy
        }
        ImGui::SameLine();
        if (ImGui::Button("Refresh")) {
            // TODO: Refresh asset listing
        }
        ImGui::SameLine();
        if (ImGui::Button("Import Asset")) {
            // TODO: Show import dialog
        }
        ImGui::Separator();

        ImGui::End();



    }

    void RenderEnhancedToolbar(LevelEditor* levelEditor)
    {
        const float toolbarHeight = 40.0f;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 6));

        ImGui::Begin("##Toolbar", NULL,
                     ImGuiWindowFlags_NoDecoration |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoScrollWithMouse);

        ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, toolbarHeight));

        // Horizontal separator between sections
        auto AddSeparator = []() {
            ImGui::SameLine(0, 12);
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
            ImGui::SameLine(0, 12);
        };

        // Save/Load section
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.15f, 0.20f, 1.0f));

            if (ImGui::Button("new")) {
                //NewLevel(editor);
            }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("New Level (Ctrl+N)");

            ImGui::SameLine();
            if (ImGui::Button("open")) {
                // TODO: Show file dialog
            }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("Open Level (Ctrl+O)");

            ImGui::SameLine();
            if (ImGui::Button("Save")) {
//                if (levelEditor->currentLevelPath.empty()) {
                    if (true) {
                    /* TODO: Show save dialog */
                } else {
                    //SaveLevel(editor, editor->currentLevelPath.c_str());
                }
            }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("Save Level (Ctrl+S)");

            ImGui::PopStyleColor();
        }
        ImGui::End();
        ImGui::PopStyleVar(3);
    }
    void RenderLevelEditorUI(LevelEditor* levelEditor)
    {


//
//        ImGui::SetNextWindowViewport(viewport->ID);
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);

        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
//        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

//        ImGui::SetNextWindowViewport(viewport->ID);

       // ImGui::Begin("LevelEditor");


        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New", "Ctrl+N"))
                {

                }
                if (ImGui::MenuItem("Open", "Ctrl+O"))
                {

                }
                if (ImGui::MenuItem("Save", "Ctrl+S"))
                {

                }

                if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
                {

                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "Alt+F4"))
                {

                }
                ImGui::EndMenu();
            }


            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Undo", "Ctrl+Z", false, printf("mi"))) { /* TODO: Implement undo */ }
                if (ImGui::MenuItem("Redo", "Ctrl+Y", false, printf("mi"))) { /* TODO: Implement redo */ }
                ImGui::Separator();
                if (ImGui::MenuItem("Cut", "Ctrl+X", false, levelEditor->hasSelection)) { /* TODO: Implement cut */ }
                if (ImGui::MenuItem("Copy", "Ctrl+C", false, levelEditor->hasSelection)) { /* TODO: Implement copy */ }
                if (ImGui::MenuItem("Paste", "Ctrl+V", false, /* TODO: Check clipboard */ false)) { /* TODO: Implement paste */ }
                if (ImGui::MenuItem("Delete", "Del", false, levelEditor->hasSelection)) {  }
                ImGui::Separator();
                if (ImGui::MenuItem("Select All", "Ctrl+A")) { /* TODO: Implement select all */ }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View")) {
//                if (ImGui::MenuItem("Scene Hierarchy", NULL, &editor->hierarchyWindowOpen)) {}
//                if (ImGui::MenuItem("Inspector", NULL, &editor->inspectorWindowOpen)) {}
//                if (ImGui::MenuItem("Asset Browser", NULL, &editor->assetBrowserWindowOpen)) {}
//                if (ImGui::MenuItem("Settings", NULL, &editor->settingsWindowOpen)) {}
                if (ImGui::MenuItem("Scene Hierarchy", NULL, printf("mi"))) {}
                if (ImGui::MenuItem("Inspector", NULL, printf("mi"))) {}
                if (ImGui::MenuItem("Asset Browser", NULL, printf("mi"))) {}
                if (ImGui::MenuItem("Settings", NULL, printf("mi"))) {}
                ImGui::Separator();
//                if (ImGui::MenuItem("Grid", NULL, &editor->viewport.showGrid)) {}
//                if (ImGui::MenuItem("Wireframe Mode", NULL, &editor->viewport.wireframeMode)) {}
                if (ImGui::MenuItem("Grid", NULL, printf("mi"))) {}
                if (ImGui::MenuItem("Wireframe Mode", NULL, printf("mi"))) {}

                ImGui::EndMenu();
            }








            ImGui::EndMainMenuBar();




        }


    }

    void RenderToolbar()
    {
        ImGui::Begin("Toolbar", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);


        ImGui::End();
    }

} // Zayn