//
// Created by socki on 3/19/2025.
//

#include "myImgui_LevelEditor.h"
#include "imgui.h"
#include <imgui_internal.h>
#include "../entities/Entity.h"
// #include "../../tools/mesh"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <__filesystem/path.h>

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
    void RenderCollisionComponentUI(Game::CollisionComponent* cc) {
        if (!cc) return;
        
        const char* colliderTypeNames[] = {"Box" };
        int currentType = static_cast<int>(cc->type);
        
        if (ImGui::Combo("Collider Type", &currentType, colliderTypeNames, IM_ARRAYSIZE(colliderTypeNames))) {
            cc->type = static_cast<Game::ColliderType>(currentType);
        }
        
        // Show properties based on collider type
        // if (cc->type == Game::COLLIDER_SPHERE) {
        //     ImGui::DragFloat("Radius", &cc->radius, 0.05f, 0.01f, 100.0f);
        // }
        if (cc->type == Game::COLLIDER_BOX) {
            ImGui::DragFloat3("Size (Half-Extents)", &cc->size.x, 0.05f, 0.01f, 100.0f);
        }
        // else if (cc->type == Game::COLLIDER_PLANE) {
        //     ImGui::DragFloat3("Normal", &cc->normal.x, 0.05f, -1.0f, 1.0f);
        //     cc->normal = Normalize(cc->normal); // Keep normal normalized
        //     ImGui::DragFloat("Offset", &cc->offset, 0.05f);
        // }
        
        // Collision flags
        ImGui::Separator();
        ImGui::Checkbox("Is Trigger", &cc->isTrigger);
        
        // Collision layers (simplified, could be expanded)
        // ImGui::DragInt("Collision Layer", &cc->collisionLayer, 1.0f, 0, 32);
        // ImGui::DragInt("Collision Mask", &cc->collisionMask, 1.0f, -1, 32);
    }

    void RenderPhysicsComponentUI(Game::PhysicsComponent* pc) {
        if (!pc) return;

        // Basic physics properties
        ImGui::Checkbox("Is Static", &pc->isStatic);
        // ImGui::Checkbox("Is Kinematic", &pc->isKinematic);

        if (!pc->isStatic) {
            ImGui::Separator();

            // Mass and forces
            // ImGui::DragFloat("Mass", &pc->mass, 0.1f, 0.1f, 1000.0f);
            ImGui::Checkbox("Use Gravity", &pc->useGravity);

            // Velocity and forces
            ImGui::DragFloat3("Velocity", &pc->velocity.x, 0.1f);
            ImGui::DragFloat3("Acceleration", &pc->acceleration.x, 0.1f);

            // Physics material properties
            // ImGui::Separator();
            // ImGui::Text("Material Properties:");
            // ImGui::DragFloat("Bounciness", &pc->bounceiness, 0.01f, 0.0f, 1.0f);
            // ImGui::DragFloat("Friction", &pc->friction, 0.01f, 0.0f, 1.0f);

            // Grounded state (read-only)
            ImGui::Separator();
            // ImGui::Text("State:");
            // ImGui::BeginDisabled();
            // ImGui::Checkbox("Is Grounded", &pc->isGrounded);
            // ImGui::EndDisabled();
            // ImGui::DragFloat("Grounded Threshold", &pc->groundedThreshold, 0.01f, 0.01f, 1.0f);
        }
    }
    void RenderPlayerControlComponentUI(Game::PlayerController* pControl) {
    }


    void RenderTransformComponentUI(Game::TransformComponent* tc) {
        if (!tc) return;
        ImGui::DragFloat3("Position", &tc->position.x, 0.1f); // Adjust if value_ptr needed for your vec3

        // vec3 eulerAngles = EulerAngles(tc->rotation); // Need EulerAngles function for your quaternion
        // eulerAngles = Degrees(eulerAngles);          // Need Degrees function for your vec3
        ImGui::DragFloat3("Rotation (Euler Deg)", &tc->rotation_euler_degrees.x, 0.1f);
            // tc->rotation = QuatFromEuler(Radians(eulerAngles)); // Need Radians and QuatFromEuler for your types

        // Edit scale directly
        ImGui::DragFloat3("Scale", &tc->scale.x, 0.05f); // Adjust if value_ptr needed
        // if (ImGui::DragFloat3("Position", glm::value_ptr(tc->position), 0.1f)) {
        //     matrix.m30 = position.x;
        //     matrix.m31 = position.y;
        //     matrix.m32 = position.z;
        //     //changed = true;
        // }
        //
        // // Convert quaternion to Euler for easier editing (optional)
        // glm::vec3 eulerAngles = glm::eulerAngles(tc->rotation);
        // eulerAngles = glm::degrees(eulerAngles); // Convert to degrees
        // if (ImGui::DragFloat3("Rotation (Euler)", glm::value_ptr(eulerAngles), 1.0f)) {
        //     tc->rotation = glm::quat(glm::radians(eulerAngles)); // Convert back to radians and then quat
        // }
        // // Alternatively, edit quaternion directly if preferred
        //
        // ImGui::DragFloat3("Scale", glm::value_ptr(tc->scale), 0.05f);
    }

    void RenderRenderComponentUI(Game::RenderComponent* rc, const std::vector<std::string>& meshNames, const std::vector<std::string>& materialNames, Engine* engine) {
        if (!rc) return; // Should not happen if called correctly, but good practice

        ImGui::PushID(rc);

        // --- Mesh Selection Dropdown ---
        int currentMeshItemIndex = 0; // Default to "[None]" or the first item
        const std::string& currentMeshName = rc->meshName; // Assuming meshName exists

        bool found = false;
        for (int i = 0; i < meshNames.size(); ++i) {
            if (meshNames[i] == currentMeshName) {
                currentMeshItemIndex = i;
                found = true;
                break;
            }
        }

        std::string previewName = "[None]"; // Default preview
        if (!currentMeshName.empty() && !found) {

            previewName = currentMeshName + " (Missing!)";
            // You might want to visually indicate the error more strongly
            // ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red text
        } else if (found && currentMeshItemIndex != 0) { // Check index if "[None]" is element 0
            previewName = meshNames[currentMeshItemIndex];
        }

        std::vector<const char*> cMeshNames;
        cMeshNames.reserve(meshNames.size());
        for (const std::string& name : meshNames) {
            cMeshNames.push_back(name.c_str());
        }

        if (ImGui::BeginCombo("Mesh", previewName.c_str())) // Use BeginCombo/EndCombo for more control
        {
            for (int i = 0; i < cMeshNames.size(); ++i) {
                // Prevent selectable with truly empty label derived from "" in meshNames
                if (cMeshNames[i] == nullptr || cMeshNames[i][0] == '\0') {
                    // Optionally display placeholder or just skip
                    // ImGui::TextDisabled("[Empty Name Entry]");
                    continue;
                }

                const bool isSelected = (currentMeshItemIndex == i);

                // Selectable ID is generated from cMeshNames[i] + PushID(rc) scope
                if (ImGui::Selectable(cMeshNames[i], isSelected)) {
                    currentMeshItemIndex = i; // Index reflects position in the loops here

                    // Update component based on selection from meshNames vector
                    if (meshNames[i] == "[None]") {
                        rc->meshName = "";
                        SetRenderComponentMesh(rc, nullptr); // Set pointer to null
                        printf("Selected mesh: [None]\n");
                    } else {
                        rc->meshName = meshNames[i];
                        // Get the pointer using the *selected* name
                        Game::Mesh* meshPtr = GetMeshPointerByName(engine, rc->meshName);
                        SetRenderComponentMesh(rc, meshPtr); // Update the component's mesh pointer
                        if(meshPtr) {
                            printf("Selected mesh: %s (Ptr: %p)\n", rc->meshName.c_str(), (void*)meshPtr);
                        } else {
                            printf("Selected mesh: %s (FAILED TO GET POINTER)\n", rc->meshName.c_str());
                        }
                    }
                }

                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }


        // --- Mesh Selection Dropdown ---
        int currentMaterialItemIndex = 0; // Default to "[None]" or the first item
        const std::string& currentMaterialName = rc->materialName;

        bool foundMaterial = false;
        for (int i = 0; i < materialNames.size(); ++i) {
            if (materialNames[i] == currentMaterialName) {
                currentMaterialItemIndex = i;
                foundMaterial = true;
                break;
            }
        }

        std::string previewNameMaterial = "[None]"; // Default preview
        if (!currentMaterialName.empty() && !found) {

            previewNameMaterial = currentMaterialName + " (Missing!)";
            // You might want to visually indicate the error more strongly
            // ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red text
        } else if (found && currentMaterialItemIndex != 0) { // Check index if "[None]" is element 0
            previewNameMaterial = materialNames[currentMaterialItemIndex];
        }

        std::vector<const char*> cMaterialNames;
        cMaterialNames.reserve(materialNames.size());
        for (const std::string& name : materialNames) {
            cMaterialNames.push_back(name.c_str());
        }

        if (ImGui::BeginCombo("Material", previewNameMaterial.c_str())) // Use BeginCombo/EndCombo for more control
        {
            for (int i = 0; i < cMaterialNames.size(); ++i) {
                // Prevent selectable with truly empty label derived from "" in meshNames
                if (cMaterialNames[i] == nullptr || cMaterialNames[i][0] == '\0') {
                    // Optionally display placeholder or just skip
                    // ImGui::TextDisabled("[Empty Name Entry]");
                    continue;
                }

                const bool isSelected = (currentMaterialItemIndex == i);

                // Selectable ID is generated from cMeshNames[i] + PushID(rc) scope
                if (ImGui::Selectable(cMaterialNames[i], isSelected)) {
                    currentMaterialItemIndex = i; // Index reflects position in the loops here

                    // Update component based on selection from meshNames vector
                    if (materialNames[i] == "[None]") {
                        rc->materialName = "";
                        SetRenderComponentMaterial(rc, nullptr); // Set pointer to null
                        printf("Selected material: [None]\n");
                    } else {
                        rc->materialName = materialNames[i];
                        // Get the pointer using the *selected* name
                        Game::Material* materialPtr = GetMaterialPointerByName(engine, rc->materialName);
                        SetRenderComponentMaterial(rc, materialPtr); // Update the component's mesh pointer
                        if(materialPtr) {
                            printf("Selected material: %s (Ptr: %p)\n", rc->materialName.c_str(), (void*)materialPtr);
                        } else {
                            printf("Selected material: %s (FAILED TO GET POINTER)\n", rc->materialName.c_str());
                        }
                    }
                }

                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::PopID();

    }

    void RenderInspectorWindow(LevelEditor* levelEditor, EntityFactory* entityFactory, Engine* engine)
    {
        ImGui::Begin("Inspector", &levelEditor->inspectorWindowOpen);

       // Game::Entity* entity = static_cast<Game::Entity*>(GetEntity(entityFactory, engine->HTEST));

        if (!levelEditor->hasSelection) {
            ImGui::TextDisabled("No entity selected");
            ImGui::End();
            return;
        }

        EntityHandle selectedHandle = levelEditor->selectedEntityHandle;
        Game::Entity* entity = static_cast<Game::Entity*>(GetEntity(entityFactory, selectedHandle));
        if (!entity) {
            ImGui::TextDisabled("Selected entity pointer is invalid!");
            levelEditor->hasSelection = false; // Deselect invalid entity
            ImGui::End();
            return;
        }


        static char nameBuffer[256];
        ImGui::PushID((void*)(intptr_t)selectedHandle.indexInInfo);
        strncpy(nameBuffer, entity->name.c_str(), sizeof(nameBuffer) - 1);
        nameBuffer[sizeof(nameBuffer) - 1] = '\0'; // Ensure null termination
        if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) {
            entity->name = nameBuffer; // Update entity name directly
        }
        ImGui::PopID();

        ImGui::Text("Handle: Idx=%d, Gen=%d, Type=%d",
                    selectedHandle.indexInInfo,
                    selectedHandle.generation,
                    (int)selectedHandle.type); // Cast enum to int for display
        ImGui::Separator();


        // --- Components Section ---
        ImGui::Text("Components");
        ImGui::Separator();

        ComponentStorage* storage = &engine->componentFactory.componentStorage;

        Game::TransformComponent* tc = FindComponentInArray(&storage->transformComponents, selectedHandle);
        Game::RenderComponent* rc = FindComponentInArray(&storage->renderComponents, selectedHandle);
        Game::CollisionComponent* cc = FindComponentInArray(&storage->collisionComponents, selectedHandle);
        Game::PhysicsComponent* pc = FindComponentInArray(&storage->physicsComponents, selectedHandle);
        // Game::PlayerController* pControl = FindComponentInArray(&storage->playerControllers, selectedHandle);
        bool showTransform = (tc != nullptr);
        bool showRender = (rc != nullptr);
        bool showCollision = (cc != nullptr);
        bool showPhysics = (pc != nullptr);
        // bool showPlayerControl = (pControl != nullptr);
        if (ImGui::CollapsingHeader("Transform", &showTransform, ImGuiTreeNodeFlags_DefaultOpen)) {
           RenderTransformComponentUI(tc);
        }
        if (ImGui::CollapsingHeader("Render", &showRender, ImGuiTreeNodeFlags_DefaultOpen)) {
            RenderRenderComponentUI(rc, engine->meshFactory.availableMeshNames, engine->materialFactory.availableMaterialNames,  engine);
        }
        if (ImGui::CollapsingHeader("Colission", &showCollision, ImGuiTreeNodeFlags_DefaultOpen)) {
            RenderCollisionComponentUI(cc);
        }
        if (ImGui::CollapsingHeader("Physics", &showPhysics, ImGuiTreeNodeFlags_DefaultOpen)) {
            RenderPhysicsComponentUI(pc);
        }
        // if (ImGui::CollapsingHeader("Player Controller", &showPlayerControl, ImGuiTreeNodeFlags_DefaultOpen)) {
            // RenderPlayerControlComponentUI(pControl);
        // }







        // Handle component removal (if header checkbox is unchecked)
        if (tc && !showTransform) {
            // TODO: Implement component removal
            // RemoveTransformComponent(storage, selectedHandle); // Need function to remove & potentially compact array/update indices
            printf("TODO: Remove Transform Component\n");
        }
        if (tc && !showRender) {
            // TODO: Implement component removal
            // RemoveRenderComponent(storage, selectedHandle); // Need function to remove & potentially compact array/update indices
            printf("TODO: Remove Render Component\n");
        }

        ImGui::Separator();
        if (ImGui::Button("Add Component")) {
            ImGui::OpenPopup("AddComponentPopup");
        }

        if (ImGui::BeginPopup("AddComponentPopup")) {
            ImGui::Text("Available Components:");
            ImGui::Separator();

            for (int i = 0; i < (int)ComponentType::ComponentType_Count; ++i) {
                ComponentType type = (ComponentType)i;
                bool alreadyHasThisType = false;
                const char* typeName = GetComponentTypeName(type);

                switch(type) {
                    case ComponentType::TransformComponent_Type: alreadyHasThisType = (tc != nullptr); break;
                    case ComponentType::RenderComponent_Type:    alreadyHasThisType = (rc != nullptr); break;
                    case ComponentType::CollisionComponent_Type:    alreadyHasThisType = (cc != nullptr); break;
                    case ComponentType::PhysicsComponent_Type:    alreadyHasThisType = (pc != nullptr); break;
                    // case ComponentType::PlayerController_Type:    alreadyHasThisType = (pControl != nullptr); break;
                    // case ComponentType::COLLISION: alreadyHas = (cc != nullptr); break;
                    // ... other types
                    case ComponentType::ComponentType_Count:
                        default: break;
                }

                if (!alreadyHasThisType) {
                    if (ImGui::MenuItem(typeName)) {
                        // Add the selected component
                        switch(type) {
                            case ComponentType::TransformComponent_Type: AddComponent(&storage->transformComponents, selectedHandle); break;
                            case ComponentType::RenderComponent_Type:    AddComponent(&storage->renderComponents, selectedHandle);    break;
                            case ComponentType::CollisionComponent_Type:    AddComponent(&storage->collisionComponents, selectedHandle);    break;
                            case ComponentType::PhysicsComponent_Type:    AddComponent(&storage->physicsComponents, selectedHandle);    break;
                            // case ComponentType::PlayerController_Type:    AddComponent(&storage->playerControllers, selectedHandle);    break;
                            // case ComponentType::COLLISION: AddComponent(&storage->collisionComponents, selectedHandle); break;

                            // ... other types
                            case ComponentType::ComponentType_Count:
                                default: break;
                        }
                        ImGui::CloseCurrentPopup();
                    }
                }


            }


            ImGui::EndPopup();
        }

        ImGui::End();
    }




    void RenderEntityNode(Engine* engine, EntityFactory* entityFactory, LevelEditor* levelEditor, EntityHandle entityHandle) {
        void* entityBasePtr = GetEntity(entityFactory, entityHandle);
        if (!entityBasePtr) { return;}
        Game::Entity* entity = static_cast<Game::Entity*>(entityBasePtr);

        bool isSelectedEntity = levelEditor->hasSelection && AreHandlesEqual(entityHandle, levelEditor->selectedEntityHandle);

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        if (isSelectedEntity) flags |= ImGuiTreeNodeFlags_Selected;



        // TODO: Check for children when hierarchy is implemented
        bool hasChildren = false;
        if (!hasChildren) flags |= ImGuiTreeNodeFlags_Leaf;

        bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)entityHandle.indexInInfo, flags, "%s", entity->name.c_str()); // Use entity name
        if (ImGui::IsItemClicked()) {
            levelEditor->selectedEntityHandle = entityHandle;
            printf("this is being clicked");
            levelEditor->hasSelection = true;
        }
        ImGui::TreePop();

    }
    // Scene hierarchy window
    void RenderSceneHierarchyWindow(LevelEditor* levelEditor, EntityFactory* entityFactory, Engine* engine) {
        ImGui::Begin("Scene Hierarchy", &levelEditor->hierarchyWindowOpen);
        // Search bar for filtering entities
        static char searchBuffer[256] = "";
        ImGui::InputText("Search", searchBuffer, sizeof(searchBuffer));
        ImGui::Separator();

        // Entity list
        for (int i = 0; i < levelEditor->currentLevelData.levelEntityHandles.count; i++)
        {
            EntityHandle entityHandle = levelEditor->currentLevelData.levelEntityHandles[i];

            Game::Entity* entity = static_cast<Game::Entity*>(GetEntity(entityFactory, levelEditor->currentLevelData.levelEntityHandles[i]));
            if (!entity) continue;
            RenderEntityNode(engine, entityFactory, levelEditor, entityHandle);


        }
        ImGui::End();
    }

    void RenderAssetGrid(LevelEditor* editor, const char* assetNames[], int assetCount) {
        // Calculate columns based on window width
        float windowWidth = ImGui::GetContentRegionAvail().x;
        float thumbnailSize = 80.0f;
        int columns = Max(1, (int)(windowWidth / (thumbnailSize + 10)));

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
                // Popup info for new level
                ImGui::OpenPopup("New Level");
            }

            if (ImGui::BeginPopup("New Level")) {
                ImGui::Text("Create New Level");
                ImGui::Separator();
                static char levelName[128] = "";
                ImGui::InputText("Level Name", levelName, IM_ARRAYSIZE(levelName));

                if (ImGui::Button("Create"))
                {
                    LevelData levelData ={};
                    levelData.name = levelName;
                    SaveLevel(levelData, engine);
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SameLine();
                if (ImGui::Button("Cancel"))
                {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            if (ImGui::Button("Open Level..."))
            {
                levelEditor->levelFiles = GetLevelFiles("../src/game/levels/");
                ImGui::OpenPopup("Open Level");

            }

            if (ImGui::BeginPopup("Open Level")) {
                ImGui::Text("Select a level to open:");
                ImGui::Separator();

                // Display a list of available level files
                for (const auto& levelFile : levelEditor->levelFiles) {
                    // Extract just the filename without path
                    std::string filename = std::filesystem::path(levelFile).filename().string();

                    if (ImGui::Selectable(filename.c_str())) {
                        // Handle level loading here
                        LoadLevel(&levelEditor->currentLevelData,  engine, levelFile);
                        ImGui::CloseCurrentPopup();
                    }
                }

                ImGui::Separator();
                if (ImGui::Button("Cancel")) {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
            ImGui::Separator();
            if (ImGui::Button("Save Level..."))
            {
                SaveLevel(levelEditor->currentLevelData, engine);
            }
            if (ImGui::Button("Save Level As..."))
            {

            }
        }

        if (ImGui::CollapsingHeader("Level Data", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Name: %s", levelEditor->currentLevelData.name.c_str());

            // Display the version
            ImGui::Text("Version: %d", levelEditor->currentLevelData.version);

            // Display the filename (with null check)
            ImGui::Text("File: %s", levelEditor->currentLevelData.fileName ? levelEditor->currentLevelData.fileName : "None");

            // Display entity count
            // ImGui::Text("Entities: %d", levelEditor->currentLevelData.levelEntityHandles.size());

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

                if (ImGui::Button("Create New Entity...")) {
                    ImGui::OpenPopup("New Entity");
                }


                if (ImGui::BeginPopup("New Entity")) {
                    ImGui::Text("Select and Entity Type to Create:");
                    ImGui::Separator();

                    const char* items[static_cast<int>(Game::EntityType::EntityType_Count)];
                    for (int i = 0; i < static_cast<int>(Game::EntityType::EntityType_Count); i++) {
                        items[i] = Game::entityTypeInfoForBuffer[i].typeName;
                    }


                    int currentSelectedItem = static_cast<int>(levelEditor->entityCreator.selectedEntityType);

                    if (ImGui::Combo("Entity Type", &currentSelectedItem, items, static_cast<int>(Game::EntityType::EntityType_Count))) {
                        // Update the selected type when changed
                        levelEditor->entityCreator.selectedEntityType = static_cast<Game::EntityType>(currentSelectedItem);
                        // selectedType = static_cast<Game::EntityType>(currentSelectedItem);
                    }

                    // Display a list of available entity types
                    // for (const auto& levelFile : levelEditor->levelFiles) {
                    //     // Extract just the filename without path
                    //     std::string filename = std::filesystem::path(levelFile).filename().string();
                    //
                    //     if (ImGui::Selectable(filename.c_str())) {
                    //         // Handle level loading here
                    //         LoadLevel(&levelEditor->currentLevelData, levelFile);
                    //         ImGui::CloseCurrentPopup();
                    //     }
                    // }

                    ImGui::Separator();
                    if (ImGui::Button("Cancel")) {
                        ImGui::CloseCurrentPopup();
                    }

                    if (ImGui::Button("Create")) {
                        ImGui::CloseCurrentPopup();
                        // Create new Entity()
                        LE::CreateEntity(engine, &engine->entityFactory, levelEditor->entityCreator, levelEditor);

                    }

                    ImGui::EndPopup();
                }

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
            if (ImGui::Button("load")) {
                // TODO: Show file dialog

            }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("Load Level (Ctrl+O)");

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
    }\
    void RenderToolbar()
    {
        ImGui::Begin("Toolbar", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);


        ImGui::End();
    }

} // Zayn