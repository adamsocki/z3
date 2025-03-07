//
// Created by socki on 3/5/2025.
//

#ifndef Z3_MATERIAL_H
#define Z3_MATERIAL_H


#include <vector>
#include "Texture.h"

namespace Game {

    enum MaterialType
    {
        MATERIAL_PBR,
        MATERIAL_UNLIT
    };

    struct Material{

        std::string name;
        MaterialType type;
        std::vector<VkDescriptorSet> descriptorSets;
        Texture* texture;
        float color[4];
        float metallic;
        float roughness;
        bool isInitialized = false;


    };

} // Game

#endif //Z3_MATERIAL_H
