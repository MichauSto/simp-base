#include "window.hpp"
#include "utils/Windows.hpp"
#include "utils/assert.hpp"

#include "core/event.hpp"
#include "winmsgtranslator.hpp"

#include <string>
#include <dwmapi.h>



#define WM_USERCLOSE (WM_USER + 1)
#define WM_USERTITLECHANGE (WM_USER + 2)
#define WM_USERWINDOWREADY (WM_USER + 3)

namespace simp {

  Window* Window::s_Instance = nullptr;

  LRESULT Window::wndProcSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
  {
    switch (msg) {
      case WM_NCCREATE:
      {
        auto args = reinterpret_cast<CREATESTRUCT*>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)args->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)&Window::wndProc);
      }
      break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
  }

  LRESULT CALLBACK Window::wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
  {
    auto window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    switch (msg) {
      case WM_USERCLOSE:
      {
        PostQuitMessage(0);
        break;
      }
      case WM_USERTITLECHANGE:
      {
        HRESULT _hResult;
        auto title = window->m_Title;
        SIMP_GETLASTERROR(SetWindowText(window->m_Handle, title.c_str()));
        return 1;
      }
      case WM_USERWINDOWREADY:
      {
        window->m_Ready = true;
        window->m_CvWindowReady.notify_all();
        break;
      }
      case WM_CLOSE:
      {
        EventDispatcher::Get().enqueue<WindowCloseEvent>();
        return 1;
      }
      case WM_SIZE:
      {
        window->m_Width = LOWORD(lParam);
        window->m_Height = HIWORD(lParam);
        EventDispatcher::Get().enqueue<WindowResizeEvent>(LOWORD(lParam), HIWORD(lParam));
        break;
      }
      default:
      {
        if (!window->m_Ready) break;
        WindowsMessageEvent e{};
        EventDispatcher::Get().trigger(e);
        break;
      }
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
  }

  const HWND& Window::getHandle()
  {
    SIMP_ASSERT(s_Instance);
    s_Instance->waitUntilReady();
    return s_Instance->m_Handle;
  }

  HWND Window::tryGetHandle()
  {
    if (!s_Instance) return nullptr;
    s_Instance->waitUntilReady();
    return s_Instance->m_Handle;
    //auto handle = s_Instance->m_Handle;
    //return IsWindow(handle) ? handle : nullptr;
  }

  bool Window::getFullscreen()
  {
    SIMP_ASSERT(s_Instance);
    return s_Instance->_getFullscreen();
  }

  void Window::setFullscreen(bool state)
  {
    SIMP_ASSERT(s_Instance);
    s_Instance->_setFullscreen(state);
  }

  void Window::setTitle(const std::string& title)
  {
    SIMP_ASSERT(s_Instance);
    s_Instance->_setTitle(title);
  }

  uint32_t Window::getWidth()
  {
    SIMP_ASSERT(s_Instance);
    return s_Instance->m_Width;
  }

  uint32_t Window::getHeight()
  {
    SIMP_ASSERT(s_Instance);
    return s_Instance->m_Height;
  }

  float Window::getRefreshInterval()
  {
    HRESULT _hResult;
    DWM_TIMING_INFO timing{};
    timing.cbSize = sizeof(timing);
    SIMP_HRESULT(DwmGetCompositionTimingInfo(nullptr, &timing));

    return timing.rateRefresh.uiDenominator / (float)timing.rateRefresh.uiNumerator;
  }

  Window::Window()
  {
    SIMP_ASSERT(!s_Instance);
    
    s_Instance = this;
    
    m_ThreadHandle = CreateThread(nullptr, 0, &Window::wndLoopStatic, this, 0, nullptr);
    waitUntilReady();
  }

  Window::~Window()
  {
    SIMP_ASSERT(s_Instance == this);
    PostMessage(m_Handle, WM_USERCLOSE, 0, 0);
    WaitForSingleObject(m_ThreadHandle, INFINITE);
    s_Instance = nullptr;
  }

  DWORD Window::wndLoopStatic(void* ptr)
  {
    return static_cast<Window*>(ptr)->wndLoop();
  }

  DWORD Window::wndLoop()
  {
    HRESULT _hResult;

    WNDCLASSEX wndClass{};
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_CLASSDC;
    wndClass.lpfnWndProc = wndProcSetup;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = GetModuleHandle(nullptr);
    wndClass.hIcon = nullptr;
    wndClass.hCursor = nullptr;
    wndClass.hbrBackground = nullptr;
    wndClass.lpszMenuName = nullptr;
    wndClass.lpszClassName = "ptWindowClass";
    wndClass.hIconSm = nullptr;

    m_WindowPlacement = {};
    m_WindowPlacement.length = sizeof(WINDOWPLACEMENT);

    SIMP_GETLASTERROR(RegisterClassEx(&wndClass));

    SIMP_GETLASTERROR(m_Handle = CreateWindow(
      wndClass.lpszClassName,
      "",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      1280,
      720,
      nullptr,
      nullptr,
      wndClass.hInstance,
      this));

    SIMP_GETLASTERROR(ShowWindow(m_Handle, SW_SHOWDEFAULT));

    PostMessage(m_Handle, WM_USERWINDOWREADY, 0, 0);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    return 0;
  }

  bool Window::_getFullscreen()
  {
    HRESULT _hResult;
    UINT style = static_cast<UINT>(GetWindowLongPtr(m_Handle, GWL_STYLE));
    SIMP_GETLASTERROR(style);
    return !(style & WS_BORDER);
  }

  void Window::_setTitle(const std::string& title)
  {
    m_Title = title;
    PostMessage(m_Handle, WM_USERTITLECHANGE, 0, 0);
  }

  void Window::_setFullscreen(bool state)
  {
    HRESULT _hResult;
    // Get current style
    UINT style = static_cast<UINT>(GetWindowLongPtr(m_Handle, GWL_STYLE));
    SIMP_GETLASTERROR(style);
    if (!(style & WS_BORDER) == state) return; // No state change
    if (state) { // Windowed to fullscreen
      // Backup window style
      m_Style = style;
      // Set borderless style flags
      style = WS_POPUP | WS_VISIBLE;
      // Backup window position
      SIMP_GETLASTERROR(GetWindowRect(m_Handle, &m_WindowBounds));
      // Backup window placement
      SIMP_GETLASTERROR(GetWindowPlacement(m_Handle, &m_WindowPlacement));
      // Get screen dimensions
      int width = GetSystemMetrics(SM_CXSCREEN);
      int height = GetSystemMetrics(SM_CYSCREEN);
      // Set window style flags
      SIMP_GETLASTERROR(SetWindowLongPtrW(m_Handle, GWL_STYLE, static_cast<LONG_PTR>(style)));
      // Set window position
      SIMP_GETLASTERROR(SetWindowPos(
        m_Handle,
        HWND_TOPMOST,
        0,
        0,
        width,
        height,
        SWP_FRAMECHANGED));
    }
    else { // Fullscreen to windowed
      // Restore style flags
      SIMP_GETLASTERROR(SetWindowLongPtrW(m_Handle, GWL_STYLE, static_cast<LONG_PTR>(m_Style)));
      // Restore window position
      SIMP_GETLASTERROR(SetWindowPos(
        m_Handle,
        HWND_NOTOPMOST,
        m_WindowBounds.left,
        m_WindowBounds.top,
        m_WindowBounds.right - m_WindowBounds.left,
        m_WindowBounds.bottom - m_WindowBounds.top,
        SWP_FRAMECHANGED));
      // Restore window placement
      SIMP_GETLASTERROR(SetWindowPlacement(m_Handle, &m_WindowPlacement));
    }
  }

  void Window::waitUntilReady()
  {
    std::unique_lock lock(m_MtxWindowReady);
    m_CvWindowReady.wait(lock, [&]() -> bool { return m_Ready; });
  }

}