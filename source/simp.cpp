#include "simp.hpp"

#include "core/event.hpp"
#include "core/window.hpp"

namespace simp {

  int LaunchSettings::argCallback(const std::string* arr, int size)
  {
    if (!_stricmp(arr[0].c_str(), "-omsi")) {
      omsiDir = arr[1];
      return 2;
    }
    return 1;
  }

  Simp::Simp(const LaunchSettings& settings)
    : MeshManager(settings.omsiDir), TextureManager(settings.omsiDir)
  {
    assert(!Instance);
    Instance = this;

    EventDispatcher::Get().sink<WindowCloseEvent>().connect<&Simp::OnClose>(this);
  }

  Simp::~Simp()
  {
    assert(Instance == this);
    Instance = nullptr;
  }

  void Simp::Run()
  {
    while (Running) {
      EventDispatcher::Get().update();

      float f = Window::getRefreshInterval();

      auto context = Graphics.GetDeviceContext();
      
      ID3D11RenderTargetView* rtv = Graphics.GetRenderTargetView();
      ID3D11DepthStencilView* dsv = Graphics.GetDepthStencilView();
      context->OMSetRenderTargets(1, &rtv, dsv);

      glm::vec4 color{ 1.f, 0.f, 1.f, 1.f };
      context->ClearRenderTargetView(Graphics.GetRenderTargetView(), (float*)&color);
      context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0.f);

      Graphics.Present();
    }
  }

  const MeshManager& Simp::GetMeshManager()
  {
    assert(Instance);
    return Instance->MeshManager;
  }

  const TextureManager& Simp::GetTextureManager()
  {
    assert(Instance);
    return Instance->TextureManager;
  }

  const Graphics& Simp::GetGraphics()
  {
    assert(Instance);
    return Instance->Graphics;
  }

  void Simp::OnClose(const WindowCloseEvent& e)
  {
    Running = false;
  }

  const Simp* Simp::Instance;

}
