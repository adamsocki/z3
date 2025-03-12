//
// Created by socki on 3/6/2025.
//

#ifndef Z3_UNIFORMBUFFEROBJECT_H
#define Z3_UNIFORMBUFFEROBJECT_H


#include <glm/glm.hpp>

namespace Game
{
    struct UniformBufferObject
    {
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 proj;
    };

}


#endif //Z3_UNIFORMBUFFEROBJECT_H
