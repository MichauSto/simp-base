#include "physicscomponents.hpp"
#include "transformcomponents.hpp"
#include "scene.hpp"
#include <glm/gtx/transform.hpp>

namespace simp {

  using namespace physx;

  void Scene::OnStaticAdded(entt::registry& registry, entt::entity entity)
  {
    auto& s = registry.get<StaticComponent>(entity);
    glm::mat4 mat{ 1.f };
    PxTransform t(PxMat44((float*)&mat));
    s.actor = PhysicsWorld.physics->createRigidStatic(t);
    PxShape* shape = PhysicsWorld.physics->createShape(PxTriangleMeshGeometry(s.mesh->GetTriangleMesh()), *PhysicsWorld.material);
    s.actor->attachShape(*shape);
    shape->release();
    PhysicsWorld.scene->addActor(*s.actor);
  }

  void Scene::OnStaticDestroyed(entt::registry& registry, entt::entity entity)
  {
    auto& s = registry.get<StaticComponent>(entity);
    PhysicsWorld.scene->removeActor(*s.actor);
    s.actor->release();
  }

  void Scene::UpdatePhysicsWorld(float dt)
  {
    {
      auto view = Registry.view<
        StaticComponent,
        TransformWorldComponent,
        TransformComponent,
        TransformDirtyComponent>();

      for (auto e : view) {
        auto [s, world, transform, dirty] = view.get(e);
        auto mat = glm::translate(300.f * glm::vec3(world.Tile - ReferenceTile, 0.f)) * transform.LocalTransform;
        PxTransform t(PxMat44((float*)&mat));
        s.actor->setGlobalPose(t);
      }
    }

    PhysicsWorld.TimeStep(dt);
  }

  PhysicsWorld::PhysicsWorld()
  {
    static PxFoundation* foundation = PxCreateFoundation(PX_PHYSICS_VERSION, allocator, callback);

    pvd = PxCreatePvd(*foundation);
    transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
    pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

    physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale(), true, pvd);
    cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, PxCookingParams(PxTolerancesScale()));

    material = physics->createMaterial(0.5f, 0.5f, 0.6f);

    PxSceneDesc desc{ physics->getTolerancesScale() };
    dispatcher = PxDefaultCpuDispatcherCreate(2);
    desc.gravity = PxVec3(0.0f, 0.0f, -9.81f);
    desc.cpuDispatcher = dispatcher;
    desc.filterShader = PxDefaultSimulationFilterShader;
    scene = physics->createScene(desc);
  }

  PhysicsWorld::~PhysicsWorld()
  {
    //After releasing PxPhysics, release the PVD
    scene->release();
    material->release();
    dispatcher->release();
    physics->release();
    cooking->release();
    pvd->release();
    transport->release();
  }

  void PhysicsWorld::TimeStep(float dt)
  {
    scene->simulate(dt);
    scene->fetchResults(true);
  }

  physx::PxTriangleMesh* PhysicsWorld::createMesh(const Mesh* mesh) const
  {
    PxTriangleMeshDesc meshDesc;
    meshDesc.flags = PxMeshFlag::e16_BIT_INDICES | PxMeshFlag::eFLIPNORMALS;
    meshDesc.points.count = mesh->Vertices.size();
    meshDesc.points.stride = sizeof(Vertex);
    meshDesc.points.data = (char*)mesh->Vertices.data() + offsetof(Vertex, Vertex::Position);

    meshDesc.triangles.count = mesh->Indices.size() / 3;
    meshDesc.triangles.stride = 3 * sizeof(mesh->Indices[0]);
    meshDesc.triangles.data = mesh->Indices.data();

    return cooking->createTriangleMesh(meshDesc, physics->getPhysicsInsertionCallback());
  }

  physx::PxConvexMesh* PhysicsWorld::createConvexMesh(const Mesh* mesh) const
  {
    PxConvexMeshDesc meshDesc{};
    meshDesc.flags = PxConvexFlag::e16_BIT_INDICES;
    meshDesc.points.count = mesh->Vertices.size();
    meshDesc.points.stride = sizeof(Vertex);
    meshDesc.points.data = (char*)mesh->Vertices.data() + offsetof(Vertex, Vertex::Position);

    meshDesc.indices.count = mesh->Indices.size() / 3;
    meshDesc.indices.stride = 3 * sizeof(mesh->Indices[0]);
    meshDesc.indices.data = mesh->Indices.data();

    return cooking->createConvexMesh(meshDesc, physics->getPhysicsInsertionCallback());
  }

}

