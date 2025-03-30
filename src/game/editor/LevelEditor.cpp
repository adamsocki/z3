//
// Created by socki on 3/19/2025.
//

#include "LevelEditor.h"

#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
#include "../../Engine.h"
#include "../entities/Entity.h"

// namespace Game {
//     struct Entity;
// }

#include "SerializationHelper.h"

namespace Zayn {

    // bool IsHandleInLevel(const DynamicArray<EntityHandle>& levelHandles, EntityHandle handleToFind) {
    //     for (u32 i = 0; i < levelHandles.count; ++i) {
    //         if (AreHandlesEqual(levelHandles[i], handleToFind)) {
    //             return true;
    //         }
    //     }
    //     return false;
    // }

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
        if (!levelData) {
            return; // Safety check
        }

        // Reset metadata
        levelData->name = "";             // Reset std::string name
        levelData->version = 0;           // Reset version (or set to default like 1)
        levelData->fileName = nullptr;    // Reset const char* fileName pointer

        // Clear the entity handle dynamic array (resets count to 0)
        // Assumes DynamicArrayClear exists and works as intended.
        DynamicArrayClear(&levelData->levelEntityHandles);
    }

    // Helper to check if a handle is in the level's list (can be slow)
    bool IsHandleInLevel(const DynamicArray<EntityHandle>& levelHandles, EntityHandle handleToFind) {
        for (u32 i = 0; i < levelHandles.count; ++i) {
            if (AreHandlesEqual(levelHandles[i], handleToFind)) {
                return true;
            }
        }
        return false;
    }

    // --- Simplified SaveLevel ---
    bool SaveLevel(LevelData levelData, Engine* engine) {
        nlohmann::json levelJson;

        // --- Save Level Metadata ---
        levelJson["name"] = levelData.name;
        levelJson["version"] = levelData.version;

        // --- Save Basic Entity Data ---
        nlohmann::json entitiesJson = nlohmann::json::array();
        for (int32 i = 0; i < levelData.levelEntityHandles.count; i++) {
            EntityHandle entityHandle = levelData.levelEntityHandles[i];
            Game::Entity* entity = static_cast<Game::Entity*>(GetEntity(&engine->entityFactory, entityHandle));
            if (!entity) continue;

            nlohmann::json entityJson;
            entityJson["id"] = i;
            entityJson["name"] = entity->name;
            // Manually create handle JSON object
            entityJson["handle"] = {
                {"indexInInfo", entityHandle.indexInInfo},
                {"generation", entityHandle.generation},
                {"type", static_cast<int>(entityHandle.type)} // Store enum as int
            };
            entitiesJson.push_back(entityJson);
        }
        levelJson["entities"] = entitiesJson;

        // --- Save Components ---
        nlohmann::json componentsJson;
        ComponentStorage* storage = &engine->componentFactory.componentStorage;

        // - Transform Components -
        nlohmann::json transformComponentsJson = nlohmann::json::array();
        for (u32 i = 0; i < storage->transformComponents.count; ++i) {
            const Game::TransformComponent& tc = storage->transformComponents[i];
            if (IsHandleInLevel(levelData.levelEntityHandles, tc.owner)) {
                nlohmann::json componentData;
                // Manually create owner handle JSON
                componentData["owner_handle"] = {
                    {"indexInInfo", tc.owner.indexInInfo},
                    {"generation", tc.owner.generation},
                    {"type", static_cast<int>(tc.owner.type)}
                };
                // Manually create position/rotation/scale JSON arrays
                componentData["position"] = {tc.position.x, tc.position.y, tc.position.z};
                componentData["rotation_euler_degrees"] = {tc.rotation_euler_degrees.x, tc.rotation_euler_degrees.y, tc.rotation_euler_degrees.z};
                // componentData["rotation"] = {tc.rotation.x, tc.rotation.y, tc.rotation.z};
                componentData["scale"] = {tc.scale.x, tc.scale.y, tc.scale.z};
                transformComponentsJson.push_back(componentData);
            }
        }
        if (!transformComponentsJson.empty()) {
            componentsJson["transform"] = transformComponentsJson;
        }

        // - Render Components -
        nlohmann::json renderComponentsJson = nlohmann::json::array();
        for (u32 i = 0; i < storage->renderComponents.count; ++i) {
            const Game::RenderComponent& rc = storage->renderComponents[i];
            if (IsHandleInLevel(levelData.levelEntityHandles, rc.owner)) {
                nlohmann::json componentData;
                componentData["owner_handle"] = {
                    {"indexInInfo", rc.owner.indexInInfo},
                    {"generation", rc.owner.generation},
                    {"type", static_cast<int>(rc.owner.type)}
                };
                componentData["meshName"] = rc.meshName;

                // componentData["meshName"] = rc.mesh ? rc.mesh->name : "";
                // componentData["materialName"] = rc.material ? rc.material->name : "";
                // componentData["isVisible"] = rc.isVisible;
                renderComponentsJson.push_back(componentData);
            }
        }
        if (!renderComponentsJson.empty()) {
            componentsJson["render"] = renderComponentsJson;
        }

        // TODO: Add loops to manually save other component types...

        levelJson["components"] = componentsJson;

        // --- Write JSON to file ---
        try {
            std::string pathPrefix = "../src/game/levels/";
            std::string fullPath = pathPrefix + levelData.name;
            if (fullPath.rfind(".zlvl") != fullPath.size() - 5) {
                fullPath += ".zlvl";
            }

            std::filesystem::path dirPath = std::filesystem::path(fullPath).parent_path();
             if (!std::filesystem::exists(dirPath)) {
                 std::filesystem::create_directories(dirPath);
             }

            std::ofstream file(fullPath);
            if (!file.is_open()) {
                printf("Error: Failed to open file for writing: %s\n", fullPath.c_str());
                return false;
            }
            file << levelJson.dump(4);
            file.close();

            printf("Level saved successfully to: %s\n", fullPath.c_str());
            // Consider managing fileName memory better if needed
            // if (levelData.fileName) free((void*)levelData.fileName);
            // levelData.fileName = strdup(fullPath.c_str());
            return true;

        } catch (const std::exception& e) {
            printf("Error saving level: %s\n", e.what());
            return false;
        }
    }


    // --- Function definitions needed by LoadLevel (implement these properly) ---
    // (These were conceptual before, provide actual implementations or ensure they exist)

    // Removes components owned by 'handle' from 'storage'
    // NOTE: Swap-remove implementation needs careful index handling if using indexed component access
    void RemoveComponentsForEntity(ComponentStorage* storage, EntityHandle handle) {
        // Transform
        for (u32 i = 0; i < storage->transformComponents.count; ) {
            if (AreHandlesEqual(storage->transformComponents[i].owner, handle)) {
                storage->transformComponents[i] = storage->transformComponents[storage->transformComponents.count - 1];
                storage->transformComponents.count--;
            } else { i++; }
        }
    }
         // Render
        // for (u32 i = 0; i < storage->renderComponents.count; ) {
        //     if (AreHandlesEqual(storage->renderComponents[i].owner, handle)) {
        //         storage->renderComponents[i] = storage->renderComponents[storage->renderComponents.count - 1];
        //         storage->renderComponents.count--;
        //     } else { i++; }
        // }
        // TODO: Add loops for other component types...
    // }

    // Deletes entity from factory and removes its components
    void DestroyEntityAndComponents(Zayn::EntityFactory* factory, Zayn::ComponentStorage* storage, Zayn::EntityHandle handle) {
         RemoveComponentsForEntity(storage, handle);
         // TODO: Implement DeleteEntity in your EntityFactory to free the handle/slot
         // DeleteEntity(factory, handle);
         printf("Placeholder: Would delete entity %d from factory\n", handle.indexInInfo);
    }

    // IMPORTANT: Need this function in EntityFactory to recreate entity with SPECIFIC handle
    bool RecreateEntityWithHandle(Zayn::EntityFactory* factory, Zayn::EntityHandle handle) {
        // TODO: Implement this in EntityFactory.
        // It needs to mark the slot specified by handle.indexInInfo as 'in use'
        // with the correct handle.generation and handle.type.
        // It should FAIL if the slot is already in use with a different generation.
        printf("Placeholder: Would attempt to recreate entity with handle Idx=%d, Gen=%d, Type=%d\n",
               handle.indexInInfo, handle.generation, (int)handle.type);
        // Return true if successful, false otherwise
        return true; // Placeholder
    }

    // --- Simplified LoadLevel ---
    bool LoadLevel(Zayn::LevelData* currentLevelData, Zayn::Engine* engine, std::string levelFile) {
        printf("Attempting to load level: %s\n", levelFile.c_str());
        Zayn::ComponentStorage* storage = &engine->componentFactory.componentStorage;
        Zayn::EntityFactory* factory = &engine->entityFactory;

        // --- 1. Clear Previous Level State ---
        printf("Clearing %u old entities...\n", currentLevelData->levelEntityHandles.count);
        for (int i = (int)currentLevelData->levelEntityHandles.count - 1; i >= 0; --i) {
            Zayn::EntityHandle oldHandle = currentLevelData->levelEntityHandles[i];
             DestroyEntityAndComponents(factory, storage, oldHandle);
        }
        ClearLevelData(currentLevelData); // Resets name, version, fileName, count
        printf("Previous level state cleared.\n");

        // --- 2. Load JSON from file ---
        nlohmann::json levelJson;
        try {
            std::ifstream file(levelFile); // Assume levelFile is correct path
            if (!file.is_open()) {
                printf("Error: Failed to open level file: %s\n", levelFile.c_str());
                return false;
            }
            file >> levelJson;
            file.close();
            printf("JSON file loaded and parsed.\n");
        } catch (const std::exception& e) {
            printf("Error loading/parsing level file: %s\n", e.what());
            return false;
        }

        // --- 3. Load Metadata ---
        try {
            currentLevelData->name = levelJson.value("name", "Unnamed Level");
            currentLevelData->version = levelJson.value("version", 1);
             // if (currentLevelData->fileName) free((void*)currentLevelData->fileName); // Manage memory if needed
            currentLevelData->fileName = strdup(levelFile.c_str());
            printf("Level metadata loaded: Name='%s', Version=%d\n", currentLevelData->name.c_str(), currentLevelData->version);
        } catch (const std::exception& e) {
             printf("Error reading level metadata: %s\n", e.what());
        }

        // --- 4. Load and Recreate Entities ---
        if (levelJson.contains("entities") && levelJson["entities"].is_array()) {
            const nlohmann::json& entitiesJson = levelJson["entities"];
            printf("Loading %zu entities...\n", entitiesJson.size());
            for (const auto& entityJson : entitiesJson) {
                try {
                    // Manually parse handle object
                    Zayn::EntityHandle handle = {};
                    handle.indexInInfo = entityJson.at("handle").at("indexInInfo").get<int32>();
                    handle.generation = entityJson.at("handle").at("generation").get<int32>();
                    handle.type = static_cast<Game::EntityType>(entityJson.at("handle").at("type").get<int>());

                    std::string name = entityJson.value("name", "Unnamed Entity");

                    // Recreate entity using the specific handle
                    if (RecreateEntityWithHandle(factory, handle)) {
                        Game::Entity* entity = static_cast<Game::Entity*>(GetEntity(factory, handle));
                        if (entity) {
                            entity->name = name; // Set the name
                        }
                        PushBack(&currentLevelData->levelEntityHandles, handle); // Add to level list
                    } else {
                         printf("  Error: Failed to recreate entity with handle Idx=%d, Gen=%d! Skipping.\n", handle.indexInInfo, handle.generation);
                    }
                } catch (const std::exception& e) {
                    printf("Error loading entity data: %s\n Json: %s\n", e.what(), entityJson.dump().c_str());
                }
            }
        }

        // --- 5. Load Components ---
        if (levelJson.contains("components") && levelJson["components"].is_object()) {
            const nlohmann::json& componentsJson = levelJson["components"];
            printf("Loading components...\n");

            // - Transform Components -
            if (componentsJson.contains("transform") && componentsJson["transform"].is_array()) {
                printf("  Loading transform components...\n");
                for (const auto& compJson : componentsJson["transform"]) {
                    try {
                        Game::TransformComponent tc = {};
                        // Manually parse owner handle
                        tc.owner.indexInInfo = compJson.at("owner_handle").at("indexInInfo").get<int32>();
                        tc.owner.generation = compJson.at("owner_handle").at("generation").get<int32>();
                        tc.owner.type = static_cast<Game::EntityType>(compJson.at("owner_handle").at("type").get<int>());
                        // Manually parse position, rotation, scale
                        if (compJson.at("position").is_array() && compJson.at("position").size() == 3) {
                            tc.position.x = compJson["position"][0].get<float>();
                            tc.position.y = compJson["position"][1].get<float>();
                            tc.position.z = compJson["position"][2].get<float>();
                        }
                         if (compJson.at("rotation_euler_degrees").is_array() && compJson.at("rotation_euler_degrees").size() == 3) {
                            tc.rotation_euler_degrees.x = compJson["rotation_euler_degrees"][0].get<float>();
                            tc.rotation_euler_degrees.y = compJson["rotation_euler_degrees"][1].get<float>();
                            tc.rotation_euler_degrees.z = compJson["rotation_euler_degrees"][2].get<float>();
                        }
                         if (compJson.at("scale").is_array() && compJson.at("scale").size() == 3) {
                             tc.scale.x = compJson["scale"][0].get<float>();
                             tc.scale.y = compJson["scale"][1].get<float>();
                             tc.scale.z = compJson["scale"][2].get<float>();
                         }

                        // Add component to storage
                        Game::TransformComponent* addedComp = AddComponent(&storage->transformComponents, tc.owner);
                        if(addedComp) { *addedComp = tc; } // Copy loaded data

                    } catch (const std::exception& e) {
                        printf("  Error loading transform component: %s\n Json: %s\n", e.what(), compJson.dump().c_str());
                    }
                }
            }

            // - Render Components -
            if (componentsJson.contains("render") && componentsJson["render"].is_array()) {
                 printf("  Loading render components...\n");
                for (const auto& compJson : componentsJson["render"]) {
                    try {
                        Game::RenderComponent rc = {};
                        rc.owner.indexInInfo = compJson.at("owner_handle").at("indexInInfo").get<int32>();
                        rc.owner.generation = compJson.at("owner_handle").at("generation").get<int32>();
                        rc.owner.type = static_cast<Game::EntityType>(compJson.at("owner_handle").at("type").get<int>());
                        // rc.isVisible = compJson.value("isVisible", true);
                        std::string meshNameFromJson = compJson.value("meshName", "");
                        // std::string materialName = compJson.value("materialName", "");

                        Game::RenderComponent* addedComp = AddComponent(&storage->renderComponents, rc.owner);
                        if(addedComp) {
                             *addedComp = rc; // Copy base data
                            addedComp->meshName = meshNameFromJson;
                            addedComp->mesh = nullptr;

                            if (!addedComp->meshName.empty()) {
                                addedComp->mesh = GetMeshPointerByName(engine, addedComp->meshName);

                                if (addedComp->mesh == nullptr) {
                                    // Warning: Mesh name was specified, but couldn't find/load the mesh pointer.
                                    // This might be okay if the mesh is optional or loaded later,
                                    // or it might indicate an error (e.g., mesh file missing).
                                    printf("  Warning: Could not find loaded mesh pointer for name '%s' during component load.\n",
                                           addedComp->meshName.c_str());
                                }
                            }

                             // TODO: Resolve mesh/material pointers using factories/managers and names
                             // addedComp->mesh = MeshFactory::GetMesh(..., meshName);
                             // addedComp->material = MaterialFactory::GetMaterial(..., materialName);
                        }
                    } catch (const std::exception& e) {
                        printf("  Error loading render component: %s\n Json: %s\n", e.what(), compJson.dump().c_str());
                    }
                }
            }

            // TODO: Add loading blocks to manually parse other component types...

        }

        printf("Level loaded successfully from: %s\n", levelFile.c_str());
        return true;
    }

    // // void ClearLevelData(LevelData* levelData) {
    // //     // Reset metadata
    // //     levelData->name = "";
    // //     levelData->version = 0;
    // //     levelData->fileName = "";
    // //     // memset(levelData->fileName, 0, sizeof(levelData->name));
    // //     //levelData->ambientLight = V3(0.2f, 0.2f, 0.2f);  // Default ambient light
    // //     //memset(levelData->skyboxTexturePath, 0, sizeof(levelData->skyboxTexturePath));
    // //
    // //     // Clear entities, but don't free memory; just reset the count
    // //     DynamicArrayClear(&levelData->levelEntityHandles);
    // // }
    //
    //
    // void CreateNewLevel(CreateNewLevelInfo info) {
    //
    // }
    //
    // bool SaveLevel(LevelData levelData, Engine* engine) {
    //
    //
    //     nlohmann::json levelJson;
    //
    //     // --- Save Level Metadata ---
    //     levelJson["name"] = levelData.name;
    //     levelJson["version"] = levelData.version;
    //
    //
    //     nlohmann::json entitiesJson = nlohmann::json::array();
    //     for (int32 i = 0; i < levelData.levelEntityHandles.count; i++) {
    //         EntityHandle entityHandle = levelData.levelEntityHandles[i];
    //         // Get base entity pointer *only* if needed (e.g., for name)
    //         Game::Entity* entity = static_cast<Game::Entity*>(GetEntity(&engine->entityFactory, entityHandle));
    //
    //         if (!entity) continue; // Skip invalid entities
    //
    //         nlohmann::json entityJson;
    //         entityJson["id"] = i; // Optional: index within the level handle list
    //         entityJson["name"] = entity->name; // Save name from Entity struct for now
    //         entityJson["handle"] = entityHandle; // Uses EntityHandle to_json helper
    //         entitiesJson.push_back(entityJson);
    //     }
    //     levelJson["entities"] = entitiesJson;
    //
    //     nlohmann::json componentsJson; // Top-level object for all components
    //     ComponentStorage* storage = &engine->componentFactory.componentStorage; // Get component storage
    //
    //     // - Transform Components -
    //     nlohmann::json transformComponentsJson = nlohmann::json::array();
    //     for (u32 i = 0; i < storage->transformComponents.count; ++i) {
    //         const Game::TransformComponent& tc = storage->transformComponents[i];
    //         // Only save if the component's owner is in this level
    //         if (IsHandleInLevel(levelData.levelEntityHandles, tc.owner)) {
    //             nlohmann::json componentData;
    //             SerializeComponent(componentData, tc); // Use helper
    //             transformComponentsJson.push_back(componentData);
    //         }
    //     }
    //     if (!transformComponentsJson.empty()) {
    //         componentsJson["transform"] = transformComponentsJson;
    //     }
    //
    // }
    //     // nlohmann::json levelJson;
    //     //
    //     // levelJson["name"]    = levelData.name;
    //     // levelJson["version"] = levelData.version;
    //     //
    //     //
    //     // nlohmann::json entitiesJson = nlohmann::json::array();
    //     //
    //     // for (int32 i = 0; i < levelData.levelEntityHandles.count; i++) {
    //     //     EntityHandle entityHandle = levelData.levelEntityHandles[i];
    //     //     Game::Entity* entity = static_cast<Game::Entity*>(GetEntity(&engine->entityFactory, entityHandle));
    //     //
    //     //     if (!entity) continue;  // Skip invalid entities
    //     //
    //     //     // Basic entity data
    //     //     nlohmann::json entityJson;
    //     //     entityJson["id"] = i;
    //     //     entityJson["name"] = entity->name;
    //     //     entityJson["handle"] = {
    //     //         {"indexInInfo", entityHandle.indexInInfo},
    //     //         {"generation", entityHandle.generation},
    //     //         {"type", entityHandle.type}
    //     //     };
    //     //
    //     //     entitiesJson.push_back(entityJson);
    //     // }
    //     //
    //     // levelJson["entities"] = entitiesJson;
    //     //
    //     // // write json to file
    //     // try {
    //     //     std::string fullPath = std::string("../src/game/levels/") + levelData.name;
    //     //     if (!fullPath.ends_with(".zlvl")) {
    //     //         fullPath += ".zlvl";
    //     //     }
    //     //
    //     //     std::filesystem::create_directories("levels");
    //     //     std::ofstream file(fullPath);
    //     //     if (!file.is_open()) {
    //     //         printf("Failed to open file for writing: %s\n", fullPath.c_str());
    //     //         return false;
    //     //     }
    //     //
    //     //     file << levelJson.dump(4);  // Pretty print with 4-space indentation
    //     //     file.close();
    //     //
    //     //     printf("Level saved successfully to: %s\n", fullPath.c_str());
    //     //     return true;
    //     //
    //     // } catch (const std::exception& e) {
    //     //     printf("Error saving level: %s\n", e.what());
    //     //     return false;
    //     // }
    //
    // // }
    //
    // bool LoadLevel(LevelData* currentLevelData, std::string levelFile) {
    //     // Read from JSON
    //     // Apply to LevelEditor
    //
    //     ClearLevelData(currentLevelData);
    //
    //     try {
    //         std::string fullPath =  levelFile;
    //         if (!fullPath.ends_with(".zlvl")) {
    //             fullPath += ".zlvl";
    //         }
    //         std::ifstream file(fullPath);
    //         if (!file.is_open()) {
    //             printf("Failed to open level file: %s\n", fullPath.c_str());
    //             return false;
    //         }
    //
    //         nlohmann::json levelJson;
    //         file >> levelJson;
    //         file.close();
    //
    //         int version = levelJson["version"];
    //         if (version > 1) {
    //             printf("Warning: Level file version (%d) is newer than supported version (1)\n", version);
    //         }
    //
    //         currentLevelData->name = levelJson["name"].get<std::string>();
    //         currentLevelData->version = levelJson["version"];
    //
    //
    //         printf("Level loaded successfully from: %s\n", fullPath.c_str());
    //         return true;
    //     } catch (const std::exception& e) {
    //         printf("Error loading level: %s\n", e.what());
    //         return false;
    //     }
    //
    // }

    void ApplyEditorTheme(LevelEditor* editor)
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        colors[ImGuiCol_Button]                 = ImVec4(0.25f, 0.25f, 0.27f, 1.00f);

    }

    void InitLevelEditor(LevelEditor* levelEditor, Engine* engine)
    {

        levelEditor->active = false;

        levelEditor->currentLevelData.name = "Untitled";
        levelEditor->currentLevelPath = "";

        levelEditor->currentLevelData.levelEntityHandles = MakeDynamicArray<EntityHandle>(&engine->permanentMemory, 100000);


        ApplyEditorTheme(levelEditor);


    }






} // Zayn

