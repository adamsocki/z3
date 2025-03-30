//
// Created by socki on 3/29/25.
//

#include "RenderComponent.h"

namespace Game {


    void SetRenderComponentMesh(RenderComponent* rc, Mesh* mesh) {

        rc->mesh = mesh;
    }

    void SetRenderComponentMaterial(RenderComponent* rc, Material* material)
    {
        rc->material = material;
    }


} // Game