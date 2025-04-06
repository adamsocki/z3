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

    std::vector<std::string> GetLevelFiles(const std::string& initialFolderPath) {
        std::vector<std::string> levelFiles;
        std::vector<std::string> foldersToTry;
        
        // Add the initial folder path
        foldersToTry.push_back(initialFolderPath);
        
        // Add platform-specific paths as fallbacks
        #ifdef WIN32
            foldersToTry.push_back("C:/dev/z3/src/game/levels/");
        #elif __APPLE__
            foldersToTry.push_back("/Users/socki/dev/z3/src/game/levels/");
        #endif
        
        // Try each folder path until we find some level files
        for (const auto& folderPath : foldersToTry) {
            try {
                printf("Trying to load levels from: %s\n", folderPath.c_str());
                if (!std::filesystem::exists(folderPath)) {
                    printf("  Directory does not exist, trying next path\n");
                    continue;
                }
                
                for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
                    if (entry.is_regular_file()) {
                        std::string path = entry.path().string();
                        // Check if file has the .zlvl extension
                        if (path.size() >= 5 && path.substr(path.size() - 5) == ".zlvl") {
                            levelFiles.push_back(path);
                        }
                    }
                }
                
                // If we found level files, return them
                if (!levelFiles.empty()) {
                    printf("  Found %zu level files\n", levelFiles.size());
                    return levelFiles;
                }
            } catch (const std::exception& e) {
                // Handle exceptions (e.g., directory doesn't exist)
                printf("  Error reading directory: %s\n", e.what());
            }
        }
        
        printf("Warning: No level files found in any directory\n");
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

        // - Collision Components -
        nlohmann::json collisionComponentsJson = nlohmann::json::array();
        for (u32 i = 0; i < storage->collisionComponents.count; ++i) {
            const Game::CollisionComponent& cc = storage->collisionComponents[i];
            if (IsHandleInLevel(levelData.levelEntityHandles, cc.owner)) {
                nlohmann::json componentData;
                componentData["owner_handle"] = {
                    {"indexInInfo", cc.owner.indexInInfo},
                    {"generation", cc.owner.generation},
                    {"type", static_cast<int>(cc.owner.type)}
                };
                componentData["type"] = static_cast<int>(cc.type);
                // componentData["radius"] = cc.radius;
                componentData["size"] = {cc.size.x, cc.size.y, cc.size.z};
                // componentData["normal"] = {cc.normal.x, cc.normal.y, cc.normal.z};
                // componentData["offset"] = cc.offset;
                componentData["isTrigger"] = cc.isTrigger;
                // componentData["collisionLayer"] = cc.collisionLayer;
                // componentData["collisionMask"] = cc.collisionMask;
                
                collisionComponentsJson.push_back(componentData);
            }
        }
        if (!collisionComponentsJson.empty()) {
            componentsJson["collision"] = collisionComponentsJson;
        }

        // - Physics Components -
        nlohmann::json physicsComponentsJson = nlohmann::json::array();
        for (u32 i = 0; i < storage->physicsComponents.count; ++i) {
            const Game::PhysicsComponent& pc = storage->physicsComponents[i];
            if (IsHandleInLevel(levelData.levelEntityHandles, pc.owner)) {
                nlohmann::json componentData;
                componentData["owner_handle"] = {
                    {"indexInInfo", pc.owner.indexInInfo},
                    {"generation", pc.owner.generation},
                    {"type", static_cast<int>(pc.owner.type)}
                };
                componentData["velocity"] = {pc.velocity.x, pc.velocity.y, pc.velocity.z};
                componentData["acceleration"] = {pc.acceleration.x, pc.acceleration.y, pc.acceleration.z};
                // componentData["mass"] = pc.mass;
                // componentData["bounceiness"] = pc.bounceiness;
                // componentData["friction"] = pc.friction;
                componentData["useGravity"] = pc.useGravity;
                // componentData["isStatic"] = pc.isStatic;
                // componentData["isKinematic"] = pc.isKinematic;
                // componentData["isGrounded"] = pc.isGrounded;
                // componentData["groundedThreshold"] = pc.groundedThreshold;
                
                physicsComponentsJson.push_back(componentData);
            }
        }
        if (!physicsComponentsJson.empty()) {
            componentsJson["physics"] = physicsComponentsJson;
        }
/*
        // - Player Controller Components -
        nlohmann::json playerControllerComponentsJson = nlohmann::json::array();
        for (u32 i = 0; i < storage->playerControllers.count; ++i) {
            const Game::PlayerController& pc = storage->playerControllers[i];
            if (IsHandleInLevel(levelData.levelEntityHandles, pc.owner)) {
                nlohmann::json componentData;
                componentData["owner_handle"] = {
                    {"indexInInfo", pc.owner.indexInInfo},
                    {"generation", pc.owner.generation},
                    {"type", static_cast<int>(pc.owner.type)}
                };
                componentData["moveSpeed"] = pc.moveSpeed;
                componentData["jumpForce"] = pc.jumpForce;
                componentData["airControl"] = pc.airControl;
                componentData["moveInput"] = {pc.moveInput.x, pc.moveInput.y};
                componentData["jumpRequested"] = pc.jumpRequested;
                componentData["canJump"] = pc.canJump;
                componentData["jumpCooldown"] = pc.jumpCooldown;
                componentData["jumpCooldownTimer"] = pc.jumpCooldownTimer;
                
                playerControllerComponentsJson.push_back(componentData);
            }
        }
        if (!playerControllerComponentsJson.empty()) {
            componentsJson["playerController"] = playerControllerComponentsJson;
        }
*/
        levelJson["components"] = componentsJson;

        // --- Write JSON to file ---
        try {
            std::vector<std::string> pathsToTry;
            
            // Try relative path first
            pathsToTry.push_back("../src/game/levels/");
            
            // Add platform-specific fallbacks
            #ifdef WIN32
                pathsToTry.push_back("C:/dev/z3/src/game/levels/");
            #elif __APPLE__
                pathsToTry.push_back("/Users/socki/dev/z3/src/game/levels/");
            #endif
            
            bool saveSuccess = false;
            
            for (const auto& pathPrefix : pathsToTry) {
                std::string fullPath = pathPrefix + levelData.name;
                if (fullPath.rfind(".zlvl") != fullPath.size() - 5) {
                    fullPath += ".zlvl";
                }
                
                printf("Attempting to save to: %s\n", fullPath.c_str());
                
                std::filesystem::path dirPath = std::filesystem::path(fullPath).parent_path();
                try {
                    if (!std::filesystem::exists(dirPath)) {
                        printf("  Directory doesn't exist, attempting to create: %s\n", dirPath.string().c_str());
                        std::filesystem::create_directories(dirPath);
                    }
                    
                    std::ofstream file(fullPath);
                    if (!file.is_open()) {
                        printf("  Error: Failed to open file for writing: %s\n", fullPath.c_str());
                        continue; // Try next path
                    }
                    
                    file << levelJson.dump(4);
                    file.close();
                    
                    printf("  Level saved successfully to: %s\n", fullPath.c_str());
                    // if (levelData.fileName) free((void*)levelData.fileName);
                    // levelData.fileName = strdup(fullPath.c_str());
                    saveSuccess = true;
                    break; // Success, so exit the loop
                }
                catch (const std::exception& pathEx) {
                    printf("  Error with path %s: %s\n", fullPath.c_str(), pathEx.what());
                    // Continue to next path
                }
            }
            
            if (!saveSuccess) {
                printf("Failed to save level to any location\n");
                return false;
            }
            
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
        
        // Render
        for (u32 i = 0; i < storage->renderComponents.count; ) {
            if (AreHandlesEqual(storage->renderComponents[i].owner, handle)) {
                storage->renderComponents[i] = storage->renderComponents[storage->renderComponents.count - 1];
                storage->renderComponents.count--;
            } else { i++; }
        }
        
        // Collision
        // for (u32 i = 0; i < storage->collisionComponents.count; ) {
        //     if (AreHandlesEqual(storage->collisionComponents[i].owner, handle)) {
        //         storage->collisionComponents[i] = storage->collisionComponents[storage->collisionComponents.count - 1];
        //         storage->collisionComponents.count--;
        //     } else { i++; }
        // }
        //
        // // Physics
        // for (u32 i = 0; i < storage->physicsComponents.count; ) {
        //     if (AreHandlesEqual(storage->physicsComponents[i].owner, handle)) {
        //         storage->physicsComponents[i] = storage->physicsComponents[storage->physicsComponents.count - 1];
        //         storage->physicsComponents.count--;
        //     } else { i++; }
        // }
        //
        // // PlayerController
        // for (u32 i = 0; i < storage->playerControllers.count; ) {
        //     if (AreHandlesEqual(storage->playerControllers[i].owner, handle)) {
        //         storage->playerControllers[i] = storage->playerControllers[storage->playerControllers.count - 1];
        //         storage->playerControllers.count--;
        //     } else { i++; }
        // }
    }

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
            // - Collision Components -
            if (componentsJson.contains("collision") && componentsJson["collision"].is_array()) {
                printf("  Loading collision components...\n");
                for (const auto& compJson : componentsJson["collision"]) {
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

                            Game::CollisionComponent* addedComp = AddComponent(&storage->collisionComponents, newOwnerHandle);
                            if (addedComp) {
                                addedComp->owner = newOwnerHandle;
                                
                                // Load collision type
                                if (compJson.contains("type")) {
                                    addedComp->type = static_cast<Game::ColliderType>(compJson["type"].get<int>());
                                }
                                
                                // Load radius for sphere collider
                                // if (compJson.contains("radius")) {
                                //     addedComp->radius = compJson["radius"].get<float>();
                                // }
                                
                                // Load size for box collider
                                if (compJson.at("size").is_array() && compJson.at("size").size() == 3) {
                                    addedComp->size.x = compJson["size"][0].get<float>();
                                    addedComp->size.y = compJson["size"][1].get<float>();
                                    addedComp->size.z = compJson["size"][2].get<float>();
                                }
                                
                                // Load normal for plane collider
                                // if (compJson.at("normal").is_array() && compJson.at("normal").size() == 3) {
                                //     addedComp->normal.x = compJson["normal"][0].get<float>();
                                //     addedComp->normal.y = compJson["normal"][1].get<float>();
                                //     addedComp->normal.z = compJson["normal"][2].get<float>();
                                // }
                                
                                // Load offset for plane collider
                                // if (compJson.contains("offset")) {
                                //     addedComp->offset = compJson["offset"].get<float>();
                                // }
                                
                                // Load flags
                                if (compJson.contains("isTrigger")) {
                                    addedComp->isTrigger = compJson["isTrigger"].get<bool>();
                                }
                                
                                // if (compJson.contains("collisionLayer")) {
                                //     addedComp->collisionLayer = compJson["collisionLayer"].get<int32>();
                                // }
                                
                                // if (compJson.contains("collisionMask")) {
                                //     addedComp->collisionMask = compJson["collisionMask"].get<int32>();
                                // }
                            } else {
                                printf("  Error: AddComponent failed for CollisionComponent (Owner: Idx=%d, Gen=%d).\n", 
                                       newOwnerHandle.indexInInfo, newOwnerHandle.generation);
                            }
                        } else {
                            printf("  Error: Could not find new handle mapping for old collision owner handle (Idx=%d, Gen=%d). Skipping component.\n",
                                   oldOwnerHandle.indexInInfo, oldOwnerHandle.generation);
                        }
                    } catch (const std::exception& e) {
                        printf("  Error parsing collision component data: %s\n Json: %s\n", e.what(), compJson.dump().c_str());
                    }
                }
            }
            
            // - Physics Components -
            if (componentsJson.contains("physics") && componentsJson["physics"].is_array()) {
                printf("  Loading physics components...\n");
                for (const auto& compJson : componentsJson["physics"]) {
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

                            Game::PhysicsComponent* addedComp = AddComponent(&storage->physicsComponents, newOwnerHandle);
                            if (addedComp) {
                                addedComp->owner = newOwnerHandle;
                                
                                // Load vectors
                                if (compJson.at("velocity").is_array() && compJson.at("velocity").size() == 3) {
                                    addedComp->velocity.x = compJson["velocity"][0].get<float>();
                                    addedComp->velocity.y = compJson["velocity"][1].get<float>();
                                    addedComp->velocity.z = compJson["velocity"][2].get<float>();
                                }
                                
                                if (compJson.at("acceleration").is_array() && compJson.at("acceleration").size() == 3) {
                                    addedComp->acceleration.x = compJson["acceleration"][0].get<float>();
                                    addedComp->acceleration.y = compJson["acceleration"][1].get<float>();
                                    addedComp->acceleration.z = compJson["acceleration"][2].get<float>();
                                }
                                
                                // Load scalar properties
                               /* if (compJson.contains("mass")) {
                                    addedComp->mass = compJson["mass"].get<float>();
                                }
                                
                                if (compJson.contains("bounceiness")) {
                                    addedComp->bounceiness = compJson["bounceiness"].get<float>();
                                }
                                
                                if (compJson.contains("friction")) {
                                    addedComp->friction = compJson["friction"].get<float>();
                                }
                                */
                                // Load boolean flags
                                if (compJson.contains("useGravity")) {
                                    addedComp->useGravity = compJson["useGravity"].get<bool>();
                                }
                                /*
                                if (compJson.contains("isStatic")) {
                                    addedComp->isStatic = compJson["isStatic"].get<bool>();
                                }
                                
                                if (compJson.contains("isKinematic")) {
                                    addedComp->isKinematic = compJson["isKinematic"].get<bool>();
                                }
                                
                                if (compJson.contains("isGrounded")) {
                                    addedComp->isGrounded = compJson["isGrounded"].get<bool>();
                                }
                                
                                if (compJson.contains("groundedThreshold")) {
                                    addedComp->groundedThreshold = compJson["groundedThreshold"].get<float>();
                                }
                                */
                            } else {
                                printf("  Error: AddComponent failed for PhysicsComponent (Owner: Idx=%d, Gen=%d).\n", 
                                       newOwnerHandle.indexInInfo, newOwnerHandle.generation);
                            }
                        } else {
                            printf("  Error: Could not find new handle mapping for old physics owner handle (Idx=%d, Gen=%d). Skipping component.\n",
                                   oldOwnerHandle.indexInInfo, oldOwnerHandle.generation);
                        }
                    } catch (const std::exception& e) {
                        printf("  Error parsing physics component data: %s\n Json: %s\n", e.what(), compJson.dump().c_str());
                    }
                }
            }

            /*
            // - Player Controller Components -
            if (componentsJson.contains("playerController") && componentsJson["playerController"].is_array()) {
                printf("  Loading player controller components...\n");
                for (const auto& compJson : componentsJson["playerController"]) {
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

                            Game::PlayerController* addedComp = AddComponent(&storage->playerControllers, newOwnerHandle);
                            if (addedComp) {
                                addedComp->owner = newOwnerHandle;
                                
                                // Load movement settings
                                if (compJson.contains("moveSpeed")) {
                                    addedComp->moveSpeed = compJson["moveSpeed"].get<float>();
                                }
                                
                                if (compJson.contains("jumpForce")) {
                                    addedComp->jumpForce = compJson["jumpForce"].get<float>();
                                }
                                
                                if (compJson.contains("airControl")) {
                                    addedComp->airControl = compJson["airControl"].get<float>();
                                }
                                
                                // Load input state
                                if (compJson.at("moveInput").is_array() && compJson.at("moveInput").size() == 2) {
                                    addedComp->moveInput.x = compJson["moveInput"][0].get<float>();
                                    addedComp->moveInput.y = compJson["moveInput"][1].get<float>();
                                }
                                
                                if (compJson.contains("jumpRequested")) {
                                    addedComp->jumpRequested = compJson["jumpRequested"].get<bool>();
                                }
                                
                                // Load jump state
                                if (compJson.contains("canJump")) {
                                    addedComp->canJump = compJson["canJump"].get<bool>();
                                }
                                
                                if (compJson.contains("jumpCooldown")) {
                                    addedComp->jumpCooldown = compJson["jumpCooldown"].get<float>();
                                }
                                
                                if (compJson.contains("jumpCooldownTimer")) {
                                    addedComp->jumpCooldownTimer = compJson["jumpCooldownTimer"].get<float>();
                                }
                            } else {
                                printf("  Error: AddComponent failed for PlayerController (Owner: Idx=%d, Gen=%d).\n", 
                                       newOwnerHandle.indexInInfo, newOwnerHandle.generation);
                            }
                        } else {
                            printf("  Error: Could not find new handle mapping for old player controller owner handle (Idx=%d, Gen=%d). Skipping component.\n",
                                   oldOwnerHandle.indexInInfo, oldOwnerHandle.generation);
                        }
                    } catch (const std::exception& e) {
                        printf("  Error parsing player controller component data: %s\n Json: %s\n", e.what(), compJson.dump().c_str());
                    }
                }
            }*/
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

