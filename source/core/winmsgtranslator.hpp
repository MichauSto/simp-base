#pragma once

#include "utils/Windows.hpp"

//#include <deque>
//#include <mutex>

namespace simp {

  struct WindowsMessageEvent {
    UINT Message;
    WPARAM WParam;
    LPARAM LParam;
    bool Handled;
  };

  struct WinMsgTranslator {
  public:
    WinMsgTranslator();
    void OnWindowsMessage(WindowsMessageEvent& e);
  private:
  };

}