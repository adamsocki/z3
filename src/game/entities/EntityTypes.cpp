//
// Created by socki on 3/14/2025.
//

#include "EntityTypes.h"
#include "PlayerEntity.h"


namespace Game {

    const EntityTypeInfoForBuffer entityTypeInfoForBuffer[] = {
            {EntityType_Player, sizeof(PlayerEntity), 10, "Player"}
    };
} // Game