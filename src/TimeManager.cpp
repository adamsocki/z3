//
// Created by socki on 3/2/2025.
//

#include "TimeManager.h"
#include "Engine.h"

namespace Zayn {


    void InitTimeManager(Engine* engine)
    {
        Time* time = &engine->timeManager.time;
        QueryPerformanceFrequency(&time->mTime.systemFrequency);
        QueryPerformanceCounter(&time->mTime.systemTime);

        time->mTime.startSystemTime = time->mTime.systemTime;
        //SeedRand(time->mTime.startSystemTime.QuadPart);

        time->systemTime = (real32)time->mTime.systemTime.QuadPart;

        time->startTime = 0.0f;
    }
    void UpdateTimeManager(Engine* engine)
    {
        Time* time = &engine->timeManager.time;
        time->mTime.prevSystemTime = time->mTime.systemTime;
        //LARGE_INTEGER
        int32 error = QueryPerformanceCounter(&time->mTime.systemTime);

        time->deltaTime = ((real64)time->mTime.systemTime.QuadPart - (real64)time->mTime.prevSystemTime.QuadPart) / (real64)time->mTime.systemFrequency.QuadPart;

        time->totalTime += time->deltaTime;
        time->frameCount = time->frameCount + 1;
        //std::cout << "framecnf: " << time->frameCount << std::endl;
//        for (int i =0; i < 123445; i++)
//        {
//            int32 a = 0;
//            a++;
//            a--;
//        }

        time->fpsTimer += time->deltaTime;


//        if (time->fpsTimer > 0.1f)
//        {
//
//            real64 avgFPS = time->frameCount / time->fpsTimer;
//            std::cout << avgFPS << std::endl;
//
//            time->fpsTimer = 0.0f;
//            time->frameCount = 0;
//
//        }
    }

} // Zayn