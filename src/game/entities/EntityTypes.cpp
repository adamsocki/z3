//
// Created by socki on 3/14/2025.
//

#include "EntityTypes.h"

#include "FloorEntity.h"
#include "GrandPianoEntity.h"
#include "PlayerEntity.h"


namespace Game {

    const EntityTypeInfoForBuffer entityTypeInfoForBuffer[] = {
            {EntityType_Player, sizeof(PlayerEntity), 10, "Player"},
            {EntityType_Floor, sizeof(FloorEntity), 10000, "Floor"},

            {EntityType_Piano, sizeof(GrandPianoEntity), 10, "Grand Piano"}
    };
} // Game