//
// Created by socki on 3/19/2025.
//

#include "myImgui_LevelEditor.h"
#include "imgui.h"
#include <imgui_internal.h>
#include "../entities/Entity.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Zayn {



    bool EditTransformMatrix(const char* label, mat4 matrix) {

        glm::vec3 position( matrix.m30, matrix.m31, matrix.m32);
//            glm::vec3 position( matrix.m30, matrix.m31, matrix.m32);
//        glm::vec3 position( matrix.m30, matrix.m31, matrix.m32);

        if (ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_DefaultOpen)) {

            if (ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f)) {
                matrix.m30 = position.x;
                matrix.m31 = position.y;
                matrix.m32 = position.z;
                //changed = true;
            }


            ImGui::TreePop();
        }

        return true;
    }

    void RenderInspectorWindow(LevelEditor* levelEditor, EntityFactory* entityFactory, Engine* engine)
    {
        ImGui::Begin("Inspector", &levelEditor->inspectorWindowOpen);

//

        Game::Entity* entity = static_cast<Game::Entity*>(GetEntity(entityFactory, engine->HTEST));


        if (!entity) {
            ImGui::TextDisabled("Selected entity is not valid");
            ImGui::End();
            return;
        }

        static char nameBuffer[256];
        strncpy(nameBuffer, entity->name.c_str(), sizeof(nameBuffer) - 1);
        if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) {
            entity->name = nameBuffer;
        }

        ImGui::Text("ID: %d (Gen: %d, Type: %d)",
                    levelEditor->selectedEntity.indexInInfo,
                    levelEditor->selectedEntity.generation,
                    levelEditor->selectedEntity.type);

        ImGui::Separator();

        // Transform
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
           // EditTransformMatrix("Matrix", entity->transform);

            // If you're using a ModelPushConstant
            if (ImGui::CollapsingHeader("Push Constants", ImGuiTreeNodeFlags_DefaultOpen)) {
                // Edit the model matrix in the push constant
                EditTransformMatrix("Model Matrix", entity->pushConstantData.model_1);


            }
            if (ImGui::CollapsingHeader("Render Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
                // Mesh selection
                if (entity->mesh) {
                    ImGui::Text("Current Mesh: %s", entity->mesh->name.c_str());
                } else {
                    ImGui::TextDisabled("No mesh assigned");
                }
            }
        }




        ImGui::End();
    }

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


        RenderInspectorWindow(levelEditor, entityFactory, engine);


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
    }

    void RenderToolbar()
    {
        ImGui::Begin("Toolbar", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);


        ImGui::End();
    }

} // Zayn