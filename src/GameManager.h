//
// Created by socki on 3/5/2025.
//

#ifndef Z3_GAMEMANAGER_H
#define Z3_GAMEMANAGER_H
#include "game/editor/LevelEditor.h"

namespace Zayn {

    struct Engine;

    struct GameManager {


        LevelData levelData;


    };



    void InitGameManager(Engine* engine);
    void UpdateGameManager(Engine* engine);

} // Zayn

#endif //Z3_GAMEMANAGER_H
