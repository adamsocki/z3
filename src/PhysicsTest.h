//
// Created by socki on 4/3/2025.
//

#ifndef Z3_PHYSICSTEST_H
#define Z3_PHYSICSTEST_H

#include "math/math.h"
#include "tools/EntityFactory.h"

namespace Zayn {

// Forward declarations
struct Engine;

struct PhysicsTest {
    EntityHandle playerHandle;
    EntityHandle floorHandle;
    EntityHandle objectHandles[10]; // For additional test objects
    int objectCount = 0;
};

void InitPhysicsTest(Engine* engine, PhysicsTest* test);
void UpdatePhysicsTest(Engine* engine, PhysicsTest* test);

} // Zayn

#endif //Z3_PHYSICSTEST_H