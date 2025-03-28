//
// Created by socki on 3/23/2025.
//

#ifndef Z3_COMPONENT_H
#define Z3_COMPONENT_H

#include "../../tools/EntityFactory.h"

namespace Game {

    struct Component {
        Zayn::EntityHandle ownerEntity;
    };

    void InitComponent(Component* component);
    void UpdateComponent(Component* component);

} // Game

#endif //Z3_COMPONENT_H
