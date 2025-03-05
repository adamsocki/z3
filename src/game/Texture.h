//
// Created by socki on 3/5/2025.
//

#ifndef Z3_TEXTURE_H
#define Z3_TEXTURE_H

#include <string>

namespace Game {

    struct Texture {


        std::string path;


        Texture(std::string& p)
        : path(p) {
        }

        Texture()
        : path(nullptr) {
        }


    };

} // Game

#endif //Z3_TEXTURE_H
