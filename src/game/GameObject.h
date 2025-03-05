//
// Created by socki on 3/5/2025.
//

#ifndef Z3_GAMEOBJECT_H
#define Z3_GAMEOBJECT_H

#include <glm/glm.hpp>
#include <string>
#include "Material.h"
#include "Mesh.h"

namespace Game
{






    struct GameObject {

        Material* material;
        Mesh* mesh;
        glm::mat4 transform;
        std::string name;

        GameObject(Material* mat, Mesh* m, const glm::mat4& trans, const std::string& n)
        : material(mat), mesh(m), transform(trans), name(n) {
            name= n;
            mesh=m;
            transform=trans;
            material=mat;
        }

        GameObject()
                : material(nullptr), mesh(nullptr), transform(glm::mat4(1.0f)), name("") {
        }
    };




} // Game

#endif //Z3_GAMEOBJECT_H
