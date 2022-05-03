#include "winmsgtranslator.hpp"
#include "event.hpp"

namespace simp {
  WinMsgTranslator::WinMsgTranslator()
  {
    EventDispatcher::Get().sink<WindowsMessageEvent>().connect<&WinMsgTranslator::OnWindowsMessage>(this);
  }

  void WinMsgTranslator::OnWindowsMessage(WindowsMessageEvent& e)
  {
    switch (e.Message) {
    default:
      break;
    }
  }

}