//
// Created by socki on 3/2/2025.
//

#ifndef Z2_TIMEMANAGER_H
#define Z2_TIMEMANAGER_H

#include "data_types.h"
#include <windows.h>

//#include "Engine.h"

namespace Zayn {

    struct Engine;

    struct MachineTime
    {
        LARGE_INTEGER startSystemTime;
        LARGE_INTEGER systemTime;
        LARGE_INTEGER systemFrequency;
        LARGE_INTEGER prevSystemTime;

    };

    struct Time
    {

        MachineTime mTime;

        real32 startTime;

        real32 systemTime;
        real32 prevSystemTime;

        real32 engineTime;

        real32 deltaTime;
        real32 totalTime;

        int32 frameCount;
        real32 fpsTimer;

    };

    struct TimeManager {
        Time time;
    };

    void InitTimeManager(Engine* engine);
    void UpdateTimeManager(Engine* engine);

} // Zayn

#endif //Z2_TIMEMANAGER_H
