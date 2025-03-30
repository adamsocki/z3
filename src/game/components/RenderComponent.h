//
// Created by socki on 3/29/25.
//

#ifndef RenderCOMPONENT_H
#define RenderCOMPONENT_H
#include "Component.h"
#include "../Mesh.h"

namespace Game {
    struct Material;

    struct RenderComponent : Component {
    Zayn::EntityHandle owner;

    Mesh* mesh = nullptr;
    std::string meshName = "";

    bool hasMesh;

    bool transformComponentAvailable;

    Material* material = nullptr;
        std::string materialName = "";



};


    void SetRenderComponentMesh(RenderComponent* rc, Mesh* mesh);
    void SetRenderComponentMaterial(RenderComponent* rc, Material* material);


} // Game

#endif //RenderCOMPONENT_H
