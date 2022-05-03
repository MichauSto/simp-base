#include "event.hpp"

namespace simp {

  EventDispatcher::EventDispatcher()
  {
    assert(!Instance);
    Instance = this;
  }

  EventDispatcher::~EventDispatcher()
  {
    assert(Instance == this);
    Instance = nullptr;
  }

  entt::dispatcher& EventDispatcher::Get()
  {
    assert(Instance);
    return Instance->Dispatcher;
  }

  EventDispatcher* EventDispatcher::Instance = nullptr;
  
}