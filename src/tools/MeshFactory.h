//
// Created by socki on 3/5/2025.
//

#ifndef Z3_MESHFACTORY_H
#define Z3_MESHFACTORY_H

#include "../game/Mesh.h"
#include "../DynamicArray.h"




namespace Zayn {

    struct MeshFactory {
        DynamicArray<Game::Mesh> meshes;
    };

    void MakeMesh(Engine* engine);
    void InitMeshFactory(Engine* engine);

} // Zayn

#endif //Z3_MESHFACTORY_H
