//
// Created by socki on 3/2/2025.
//

#ifndef Z2_MEMORYMANAGER_H
#define Z2_MEMORYMANAGER_H



#include "data_types.h"

#define Bytes(n) (n)
#define Kilobytes(n) (1024 * Bytes(n))
#define Megabytes(n) (1024 * Kilobytes(n))
#define Gigabytes(n) (1024 * Megabytes(n))



namespace Zayn {

    struct MAllocator;
    struct Engine;

    typedef void* AllocateFunc(MAllocator* allocator, uint64 size);
    typedef void DeallocateFunc(MAllocator* allocator, void* size);

    struct MAllocator
    {
        AllocateFunc* allocate;
        DeallocateFunc* deallocate;
    };

    struct MemoryArena : MAllocator
    {
        uint32 capacity;
        uint32 size; // in bytes
        void* ptr;
    };


    inline void* AllocateMem(MAllocator* allocator, uint64 size)
    {
        return allocator->allocate(allocator, size);
    }



    inline void DeallocateMem(MAllocator* allocator, void* data) {
        if (allocator->deallocate == nullptr) { return; }
        allocator->deallocate(allocator, data);
    }


    void AllocateMemoryArena(MemoryArena* arena, uint64 capacity);
    void* PushSizeMemoryArena(MemoryArena* arena, uint64 size);


    struct MemoryManager {

    };

    void InitMemoryManager(Engine* engine);
    void UpdateMemoryManager(Engine* engine);

} // Zayn

#endif //Z2_MEMORYMANAGER_H
