#pragma once

#include "visual/mesh.hpp"

#include <PxConfig.h>
#include <PxPhysicsAPI.h>

namespace simp {

  struct StaticComponent {
    std::shared_ptr<Mesh> mesh;
    physx::PxRigidStatic* actor;
  };

  struct PhysicsWorld {
    PhysicsWorld();
    ~PhysicsWorld();
    void TimeStep(float dt);
    physx::PxTriangleMesh* createMesh(const Mesh* mesh) const;
    physx::PxConvexMesh* createConvexMesh(const Mesh* mesh) const;
    physx::PxDefaultAllocator allocator{};
    physx::PxDefaultErrorCallback callback{};
    physx::PxDefaultCpuDispatcher* dispatcher;
    physx::PxPhysics* physics;
    physx::PxCooking* cooking;
    physx::PxPvd* pvd; 
    physx::PxPvdTransport* transport;
    physx::PxMaterial* material;
    physx::PxScene* scene;
  };

}