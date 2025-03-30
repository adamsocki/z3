//
// Created by socki on 3/5/2025.
//

#ifndef Z3_MATERIALFACTORY_H
#define Z3_MATERIALFACTORY_H

#include "../game/Material.h"
#include "../RenderManager.h"
#include "../DynamicArray.h"


namespace Zayn {


    struct MaterialCreateInfo
    {
        Game::MaterialType type = Game::MATERIAL_PBR;
        Game::Texture* texture;
        std::string name;
        float color[4];
        float roughness;
        float metallic;
    };

    struct MaterialFactory {
        DynamicArray<Game::Material> materials;

        std::unordered_map<std::string, Game::Material*> materialNamePointerMap;
        std::vector<std::string> availableMaterialNames;

    };

    void MakeMaterial(Engine* engine, MaterialCreateInfo* info);
    void InitMaterialFactory(Engine* engine);

    Game::Material* GetMaterialPointerByName(Engine* engine, const std::string& name);

} // Zayn

#endif //Z3_MATERIALFACTORY_H
