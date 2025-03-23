//
// Created by socki on 3/19/2025.
//

#ifndef Z3_LEVELEDITOR_H
#define Z3_LEVELEDITOR_H

#include <string>
#include "../../tools/EntityFactory.h"
#include  "../CameraManager.h"
#include "imgui.h"

#include "../../DynamicArray.h"
#include "../../math/math.h"


namespace Zayn {


    struct EditorColorTheme {
        ImVec4 accent;            // Primary accent color
        ImVec4 accentDark;        // Darker version of accent
        ImVec4 accentLight;       // Lighter version of accent
        ImVec4 background;        // Main background
        ImVec4 backgroundDark;    // Darker background for contrast
        ImVec4 backgroundLight;   // Lighter background for elements
        ImVec4 text;              // Primary text color
        ImVec4 textDimmed;        // Secondary text color
        ImVec4 border;            // Border color
        ImVec4 success;           // Success/green indicators
        ImVec4 warning;           // Warning/yellow indicators
        ImVec4 error;             // Error/red indicators
        ImVec4 highlight;         // Selection highlight
    };




    struct LevelData
    {
        std::string name;
        DynamicArray<EntityHandle> levelEntityHandles;
    };


    enum GizmoMode {
        GizmoMode_Translate,
        GizmoMode_Rotate,
        GizmoMode_Scale
    };

    struct HistoryBuffer {
        // @todo: memory arena implementation
//        std::vector<EditorAction> undoStack;
//        std::vector<EditorAction> redoStack;
    };

    // Gizmo
    enum SnapMode {
        SnapMode_None,
        SnapMode_Grid,
        SnapMode_Vertex
    };

    struct GridSettings {
        bool visible;
        float cellSize;
        int32 majorLineInterval;
        vec4 minorLineColor;
        vec4 majorLineColor;
        int32 size;
    };

    struct ViewportSetting {
        bool showGrid;
        bool showGizmos;
        bool wireframeMode;
        bool lockCamera;
    };





    struct LevelEditor{
        bool active;


        LevelData currentLevel;
        std::string currentLevelPath;
        bool unsavedChanges;

        DynamicArray<EntityHandle> entityHandles;
        int32 entityCount = 0;


        EntityHandle selectedEntity;
        bool hasSelection;

        // UI Conditions
        bool hierarchyWindowOpen;
        bool inspectorWindowOpen;
        bool assetBrowserWindowOpen;
        bool viewportWindowOpen;
        bool settingsWindowOpen;

        // Grid
        GridSettings grid;

        // Viewport
        ViewportSetting viewport;

        // History for undo / redo
        HistoryBuffer history;

        GizmoMode gizmoMode;
        SnapMode snapMode;
        float snapValue;

        // imgui editor styles
        EditorColorTheme theme;

        Game::CameraManager camera;

        std::string currentAssetDirectory;


    };


    void InitLevelEditor  (LevelEditor* levelEditor);
    void UpdateLevelEditor(LevelEditor* levelEditor);
    void ToggleLevelEditor(LevelEditor* levelEditor);

    void UpdateEditorCamera();

    void SelectEntity();
    void DeselectEntity();

    //LevelEntity* GetSelectedLevelEntity();   // maybe...
    void CreateEntityInEditor();
    void DeleteSelectedEntity();

    bool SaveLevelInEditor();
    bool NewLevel();







} // Zayn

#endif //Z3_LEVELEDITOR_H
