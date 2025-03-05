//
// Created by socki on 3/5/2025.
//

#ifndef Z3_MATERIAL_H
#define Z3_MATERIAL_H


#include "Texture.h"

namespace Game {

    struct Material {
        Texture* texture;


        Material(Texture* t)
        : texture(t) {
        }

        Material()
                : texture(nullptr) {
        }
    };

} // Game

#endif //Z3_MATERIAL_H
