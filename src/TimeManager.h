//
// Created by socki on 3/2/2025.
//

#ifndef Z2_TIMEMANAGER_H
#define Z2_TIMEMANAGER_H

#include "data_types.h"

#ifdef WIN32
#include <windows.h>


#elif defined(__APPLE__)
// Define LARGE_INTEGER equivalent for Apple
typedef struct {
    uint64_t QuadPart;
} LARGE_INTEGER;

#endif


//#include "Engine.h"

namespace Zayn {

    struct Engine;

    struct MachineTime
    {
        LARGE_INTEGER startSystemTime;
        LARGE_INTEGER systemTime;
        LARGE_INTEGER systemFrequency;
        LARGE_INTEGER prevSystemTime;

#ifdef __APPLE__
        double conversionFactor;
#endif

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
