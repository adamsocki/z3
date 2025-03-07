//
// Created by socki on 3/5/2025.
//

#include "MeshFactory.h"

namespace Zayn {





    void MakeMesh(Engine* engine, Game::Mesh* mesh)
    {

    }


    void InitMeshFactory(Engine* engine)
    {
        DynamicArray<Game::Mesh> meshes = MakeDynamicArray<Game::Mesh>(&engine->permanentMemory, 100);
    }

} // Zayn