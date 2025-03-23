//
// Created by socki on 3/19/2025.
//

#ifndef Z3_MYIMGUI_LEVELEDITOR_H
#define Z3_MYIMGUI_LEVELEDITOR_H

#include "LevelEditor.h"
#include "../../Engine.h"
#include "../../tools/EntityFactory.h"

namespace Zayn {

    struct myImgui_LevelEditor {


    };

    void MyRenderLevelEditorWindow(LevelEditor* levelEditor, EntityFactory* entityFactory, Engine* engine);

    void RenderLevelEditorUI(LevelEditor* levelEditor);
    void RenderEnhancedToolbar(LevelEditor* levelEditor);

} // Zayn

#endif //Z3_MYIMGUI_LEVELEDITOR_H
