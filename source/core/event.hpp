#pragma once

#include <entt/entt.hpp>

namespace simp {

  struct WindowCloseEvent {

  };

  struct WindowResizeEvent {
    uint32_t Width;
    uint32_t Height;
  };

  struct EventDispatcher {
  public:
    EventDispatcher();
    ~EventDispatcher();
    static entt::dispatcher& Get();
  private:
    static EventDispatcher* Instance;
    entt::dispatcher Dispatcher{};
  };

}