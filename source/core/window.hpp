#pragma once

#include "utils/windows.hpp"
#include <condition_variable>
#include <mutex>

namespace simp {

  struct Window {
    //virtual void onAttach() override;
    //virtual void onDetach() override;
    //virtual void onUpdate(const float& dt) override;
    static const HWND& getHandle();
    static HWND tryGetHandle();
    static bool getFullscreen();
    static void setFullscreen(bool state);
    static void setTitle(const std::string& title);
    static uint32_t getWidth();
    static uint32_t getHeight();
    static float getRefreshInterval();
    Window();
    ~Window();
  private:
    static LRESULT CALLBACK wndProcSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static Window* s_Instance;
    static DWORD wndLoopStatic(void*);
    HANDLE m_ThreadHandle;
    DWORD wndLoop();
    bool _getFullscreen();
    std::string m_Title;
    void _setTitle(const std::string& title);
    void _setFullscreen(bool state);
    void waitUntilReady();
    bool m_Ready = false;
    std::condition_variable m_CvWindowReady;
    std::mutex m_MtxWindowReady;
    HWND m_Handle;
    RECT m_WindowBounds;
    WINDOWPLACEMENT m_WindowPlacement;
    UINT m_Style;
    uint32_t m_Width;
    uint32_t m_Height;
  };

}