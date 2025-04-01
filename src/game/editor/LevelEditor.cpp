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
                componentData["materialName"] = rc.materialName;

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
        printf("Placeholder: Would attept to recreate entity with handle Idx=%d, Gen=%d, Type=%d\n",
               handle.indexInInfo, handle.generation, (int)handle.type);
        // Return true if successful, false otherwise
        return true; // Placeholder
    }

    // --- Simplified LoadLevel ---
    bool LoadLevel(Zayn::LevelData* currentLevelData, Zayn::Engine* engine, std::string levelFile) {
        printf("Attempting to load level: %s\n", levelFile.c_str());
        Zayn::ComponentStorage* storage = &engine->componentFactory.componentStorage;
        Zayn::EntityFactory* factory = &engine->entityFactory;

        std::map<Zayn::EntityHandle, Zayn::EntityHandle> oldToNewHandleMap;

        // --- Temporary storage for entity data read from JSON ---
        struct LoadedEntityData {
            Zayn::EntityHandle oldHandle;
            std::string name;
            // Add other basic entity properties if needed
        };
        std::vector<LoadedEntityData> loadedEntities;

        // --- 1. Clear Previous Level State ---
        printf("Clearing %u old entities...\n", currentLevelData->levelEntityHandles.count);
        for (int i = (int)currentLevelData->levelEntityHandles.count - 1; i >= 0; --i) {
            Zayn::EntityHandle handleToDelete = currentLevelData->levelEntityHandles[i];
             DestroyEntityAndComponents(factory, storage, handleToDelete);
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
             if (currentLevelData->fileName) free((void*)currentLevelData->fileName); // Manage memory if needed
            currentLevelData->fileName = strdup(levelFile.c_str());
            printf("Level metadata loaded: Name='%s', Version=%d\n", currentLevelData->name.c_str(), currentLevelData->version);
        } catch (const std::exception& e) {
             printf("Error reading level metadata: %s\n", e.what());
        }

        // --- 4. Load and Recreate Entities ---
        if (levelJson.contains("entities") && levelJson["entities"].is_array()) {
            const nlohmann::json& entitiesJson = levelJson["entities"];
            printf("Loading %zu entities...\n", entitiesJson.size());
            loadedEntities.reserve(entitiesJson.size());

            for (const auto& entityJson : entitiesJson) {
                try {
                    LoadedEntityData entityData;
                    // Parse the OLD handle from the file
                    entityData.oldHandle.indexInInfo = entityJson.at("handle").at("indexInInfo").get<int32>();
                    entityData.oldHandle.generation = entityJson.at("handle").at("generation").get<int32>();
                    entityData.oldHandle.type = static_cast<Game::EntityType>(entityJson.at("handle").at("type").get<int>());
                    entityData.name = entityJson.value("name", "Unnamed Entity");

                    loadedEntities.push_back(entityData);

                } catch (const std::exception& e) {
                    printf("Error loading entity data: %s\n Json: %s\n", e.what(), entityJson.dump().c_str());
                }
            }
        } else {
            printf("warning No entities array found in level file.\n");
        }


        // --- 5. Create New Entities and Build Handle Map ---
        printf("Creating %zu entities and building handle map...\n", loadedEntities.size());
        for (const auto& loadedData : loadedEntities) {
            Zayn::EntityHandle newEntityHandle = {}; // Handle to receive the newly generated one
            // Use AddEntity to get a fresh handle from the current factory state
            AddEntity(factory, &newEntityHandle, loadedData.oldHandle.type); // Use type from old handle
            Game::Entity* entity = static_cast<Game::Entity*>(GetEntity(factory, newEntityHandle));
            if (entity) {
                entity->name = loadedData.name; // Apply loaded name
                entity->handle = newEntityHandle; // Store the NEW handle in the entity itself
                // Apply other loaded basic entity properties here if needed
            } else {
                printf("  Error: Failed to get entity pointer after AddEntity for new handle Idx=%d, Gen=%d! Skipping map entry.\n", newEntityHandle.indexInInfo, newEntityHandle.generation);
                continue; // Skip adding to map and level data if entity creation failed internally
            }

            PushBack(&currentLevelData->levelEntityHandles, newEntityHandle);
            oldToNewHandleMap[loadedData.oldHandle] = newEntityHandle;
        }

        // --- 5. Load Components and Remap Owners ---
        if (levelJson.contains("components") && levelJson["components"].is_object()) {
            const nlohmann::json& componentsJson = levelJson["components"];
            printf("Loading components...\n");

            // - Transform Components -
            if (componentsJson.contains("transform") && componentsJson["transform"].is_array()) {
                printf("  Loading transform components...\n");
                for (const auto& compJson : componentsJson["transform"]) {
                    try {

                        // Parse OLD owner handle
                        Zayn::EntityHandle oldOwnerHandle = {};
                        oldOwnerHandle.indexInInfo = compJson.at("owner_handle").at("indexInInfo").get<int32>();
                        oldOwnerHandle.generation = compJson.at("owner_handle").at("generation").get<int32>();
                        oldOwnerHandle.type = static_cast<Game::EntityType>(compJson.at("owner_handle").at("type").get<int>());

                        // Find the NEW owner handle using the map
                        auto mapIt = oldToNewHandleMap.find(oldOwnerHandle);
                        if (mapIt != oldToNewHandleMap.end()) {

                            Zayn::EntityHandle newOwnerHandle = mapIt->second;

                            Game::TransformComponent* addedComp = AddComponent(&storage->transformComponents, newOwnerHandle);
                            if(addedComp) {
                                addedComp->owner = newOwnerHandle;

                                if (compJson.at("position").is_array() && compJson.at("position").size() == 3) {
                                    addedComp->position.x = compJson["position"][0].get<float>();
                                    addedComp->position.y = compJson["position"][1].get<float>();
                                    addedComp->position.z = compJson["position"][2].get<float>();
                                }
                                if (compJson.at("rotation_euler_degrees").is_array() && compJson.at("rotation_euler_degrees").size() == 3) {
                                    addedComp->rotation_euler_degrees.x = compJson["rotation_euler_degrees"][0].get<float>();
                                    addedComp->rotation_euler_degrees.y = compJson["rotation_euler_degrees"][1].get<float>();
                                    addedComp->rotation_euler_degrees.z = compJson["rotation_euler_degrees"][2].get<float>();
                                }
                                if (compJson.at("scale").is_array() && compJson.at("scale").size() == 3) {
                                    addedComp->scale.x = compJson["scale"][0].get<float>();
                                    addedComp->scale.y = compJson["scale"][1].get<float>();
                                    addedComp->scale.z = compJson["scale"][2].get<float>();
                                }

                            } else {
                                printf("  Error: AddComponent failed for TransformComponent (Owner: Idx=%d, Gen=%d).\n", newOwnerHandle.indexInInfo, newOwnerHandle.generation);
                            }
                        } else {
                            // This indicates an inconsistency (component owner handle in file doesn't match any loaded entity's old handle)
                            printf("  Error: Could not find new handle mapping for old transform owner handle (Idx=%d, Gen=%d). Skipping component.\n",
                                   oldOwnerHandle.indexInInfo, oldOwnerHandle.generation);
                        }
                    } catch (const std::exception& e) {
                        printf("  Error parsing transform component data: %s\n Json: %s\n", e.what(), compJson.dump().c_str());
                    }
                }
            }


            // - Render Components -
            if (componentsJson.contains("render") && componentsJson["render"].is_array()) {
             printf("  Loading render components...\n");
            for (const auto& compJson : componentsJson["render"]) {
                try {
                    // Parse OLD owner handle
                    Zayn::EntityHandle oldOwnerHandle = {};
                    oldOwnerHandle.indexInInfo = compJson.at("owner_handle").at("indexInInfo").get<int32>();
                    oldOwnerHandle.generation = compJson.at("owner_handle").at("generation").get<int32>();
                    oldOwnerHandle.type = static_cast<Game::EntityType>(compJson.at("owner_handle").at("type").get<int>());

                    // Find the NEW owner handle using the map
                    auto mapIt = oldToNewHandleMap.find(oldOwnerHandle);
                    if (mapIt != oldToNewHandleMap.end()) {
                         Zayn::EntityHandle newOwnerHandle = mapIt->second;

                         Game::RenderComponent* addedComp = AddComponent(&storage->renderComponents, newOwnerHandle);
                         if(addedComp) {
                             // Ensure owner field is the new handle
                             addedComp->owner = newOwnerHandle;

                             // Load names - pointers will be resolved later
                             addedComp->meshName = compJson.value("meshName", "");
                             addedComp->materialName = compJson.value("materialName", "");
                             addedComp->mesh = nullptr;     // Reset pointers
                             addedComp->material = nullptr;

                             // Load other render component properties if any (e.g., isVisible)

                         } else {
                              printf("  Error: AddComponent failed for RenderComponent (Owner: Idx=%d, Gen=%d).\n", newOwnerHandle.indexInInfo, newOwnerHandle.generation);
                         }
                    } else {
                         printf("  Error: Could not find new handle mapping for old render owner handle (Idx=%d, Gen=%d). Skipping component.\n",
                                oldOwnerHandle.indexInInfo, oldOwnerHandle.generation);
                    }
                } catch (const std::exception& e) {
                    printf("  Error loading render component: %s\n Json: %s\n", e.what(), compJson.dump().c_str());
                }
            }
        }
            // TODO: Add loading blocks to manually parse other component types...

        }

        printf("Resolving resource pointers (Meshes/Materials)...\n");
        for(u32 i = 0; i < storage->renderComponents.count; ++i) {
            Game::RenderComponent* rc = &storage->renderComponents[i];
            // Only resolve if the component actually belongs to an entity in the *currently loaded level*
            // (Optional check, but good if components from other sources might exist)
            // if (!IsHandleInLevel(currentLevelData->levelEntityHandles, rc->owner)) continue;

            if (!rc->meshName.empty() && rc->mesh == nullptr) {
                rc->mesh = GetMeshPointerByName(engine, rc->meshName);
                if (rc->mesh == nullptr) {
                    printf("  Warning: Could not find mesh '%s' for entity (Handle Idx=%d, Gen=%d).\n",
                           rc->meshName.c_str(), rc->owner.indexInInfo, rc->owner.generation);
                }
            }
            if (!rc->materialName.empty() && rc->material == nullptr) {
                rc->material = GetMaterialPointerByName(engine, rc->materialName);
                if (rc->material == nullptr) {
                    printf("  Warning: Could not find material '%s' for entity (Handle Idx=%d, Gen=%d).\n",
                           rc->materialName.c_str(), rc->owner.indexInInfo, rc->owner.generation);
                }
            }
        }

        printf("Level loaded successfully from: %s\n", levelFile.c_str());
        return true;
    }

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

