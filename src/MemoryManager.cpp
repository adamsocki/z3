//
// Created by socki on 3/2/2025.
//

#include <iostream>
#include <cstring>

#include "Engine.h"



#include "MemoryManager.h"


namespace Zayn {



    void AllocateMemoryArena(MemoryArena *arena, uint64 capacity)
    {
        std::cout << "AllocateMemoryArena" << std::endl;
        void* (*allocate)(MemoryArena*, uint64) = &PushSizeMemoryArena;
        arena->allocate = (AllocateFunc*)allocate;
        arena->deallocate = NULL;
        arena->capacity = capacity;
        arena->size = 0;
        arena->ptr = malloc(capacity);
        memset(arena->ptr, 0, capacity);

    }



    void* PushSizeMemoryArena(MemoryArena* arena, uint64 size)
    {
        void* result = (uint8*)arena->ptr + arena->size;
        arena->size += size;

        //ASSERT(arena->size < arena->capacity);

        return result;
    }


    void InitMemoryManager(Engine* engine)
    {
        AllocateMemoryArena(&engine->frameMemory, Megabytes(32));
        AllocateMemoryArena(&engine->permanentMemory, Megabytes(32));
    }

    void UpdateMemoryManager(Engine* engine)
    {

    }
} // Zayn