//
// Created by socki on 3/19/2025.
//

#ifndef Z3_LEVELEDITOR_H
#define Z3_LEVELEDITOR_H

#include <string>
#include "../../tools/EntityFactory.h"
#include  "../CameraManager.h"
#include "imgui.h"

// Forward declaration only - no include
namespace Zayn {
    struct Engine;
}

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
        int32 version = 1;
        const char* fileName;
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



        LevelData currentLevelData;
        std::string currentLevelPath;
        bool unsavedChanges;

        // DynamicArray<EntityHandle> entityHandles;
        int32 entityCount = 0;


        EntityHandle selectedEntityHandle;
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

        std::vector<std::string> levelFiles;

        LE::EntityCreator entityCreator;


    };


    void InitLevelEditor  (LevelEditor* levelEditor, Engine* engine);
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


    struct CreateNewLevelInfo {
        char levelName[128] = "";
    };

    // struct LevelDataInfo {
    //     char levelName[128] = "";
    // };
    void CreateNewLevel(CreateNewLevelInfo info);


    bool SaveLevel(LevelData levelData, Engine* engine);
    bool LoadLevel(Zayn::LevelData* currentLevelData, Zayn::Engine* engine, std::string levelFile);

    // bool LoadLevel(LevelData* currentLevelData, std::string levelFile);
    std::vector<std::string> GetLevelFiles(const std::string& folderPath);









} // Zayn

#endif //Z3_LEVELEDITOR_H
