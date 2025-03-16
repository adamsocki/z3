//
// Created by socki on 3/14/2025.
//

#ifndef Z3_ENTITYTYPES_H
#define Z3_ENTITYTYPES_H

#include <stddef.h>
#include "../../data_types.h"
//#include "Entity.h"
//#include "PlayerEntity.h"

namespace Game {

    enum EntityType {
        EntityType_Player,
        EntityType_Count
    };

    struct EntityTypeInfoForBuffer
    {
        EntityType type;
        int32 structSize;
        int32 defaultCapacity;
        const char* typeName;
    };





    extern const EntityTypeInfoForBuffer entityTypeInfoForBuffer[];


} // Game

#endif //Z3_ENTITYTYPES_H
