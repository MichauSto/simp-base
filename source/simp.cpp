#include "simp.hpp"

#include "core/event.hpp"
#include "core/window.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

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
    : OmsiPath(settings.omsiDir), 
    MeshManager(settings.omsiDir), 
    TextureManager(settings.omsiDir)
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

  void Simp::Run(glm::mat4 position, glm::ivec2 tile)
  {
    struct {
      alignas(16) glm::mat4 viewProj;
      alignas(16) glm::vec3 eye;
    } view;

    Scene.SetReferenceTile(tile);

    ComPtr<ID3D11Buffer> viewBuffer{};

    D3D11_BUFFER_DESC vbDesc{};
    vbDesc.ByteWidth = sizeof(view);
    vbDesc.Usage = D3D11_USAGE_DYNAMIC;
    vbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbDesc.MiscFlags = 0;
    vbDesc.StructureByteStride = 0;

    Graphics.GetDevice()->CreateBuffer(&vbDesc, nullptr, &viewBuffer);


    ComPtr<ID3D11DepthStencilState> dsState;

    D3D11_DEPTH_STENCIL_DESC dsDesc{};
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    dsDesc.StencilEnable = false;
    
    Graphics.GetDevice()->CreateDepthStencilState(&dsDesc, &dsState);


    ComPtr<ID3D11RasterizerState> rsState;

    D3D11_RASTERIZER_DESC rsDesc{};
    rsDesc.FillMode = D3D11_FILL_SOLID;
    rsDesc.CullMode = D3D11_CULL_BACK;
    rsDesc.FrontCounterClockwise = false;

    Graphics.GetDevice()->CreateRasterizerState(&rsDesc, &rsState);

    float lon = 0.f;

    while (Running) {
      EventDispatcher::Get().update();

      float dt = Window::getRefreshInterval();

      lon += .25f * dt;
      lon -= glm::floor(lon / glm::two_pi<float>()) * glm::two_pi<float>();

      view.eye = position * glm::eulerAngleZ(lon) * glm::vec4(0.f, -15.f, 2.f, 1.f);
      glm::vec3 target = position * glm::vec4{ 0.f, 0.f, 1.f, 1.f };
      
      view.viewProj = glm::perspectiveFovRH_ZO(.785398f, (float)Window::getWidth(), (float)Window::getHeight(), .1f, 1000.f) * glm::lookAtRH(view.eye, target, glm::vec3{ 0.f, 0.f, 1.f });



      auto context = Graphics.GetDeviceContext();

      Scene.Update(dt);

      {
        D3D11_MAPPED_SUBRESOURCE msrView{};
        context->Map(viewBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msrView);

        *static_cast<decltype(view)*>(msrView.pData) = view;

        context->Unmap(viewBuffer.Get(), 0);
      }
      
      ID3D11RenderTargetView* rtv = Graphics.GetRenderTargetView();
      ID3D11DepthStencilView* dsv = Graphics.GetDepthStencilView();
      context->OMSetRenderTargets(1, &rtv, dsv);

      glm::vec4 color{ 1.f, 0.f, 1.f, 1.f };
      context->ClearRenderTargetView(Graphics.GetRenderTargetView(), (float*)&color);
      context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

      D3D11_VIEWPORT vp{};
      vp.Width = Window::getWidth();
      vp.Height = Window::getHeight();
      vp.MinDepth = 0.f;
      vp.MaxDepth = 1.f;
      context->RSSetViewports(1, &vp);

      context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
      
      context->VSSetConstantBuffers(0, 1, viewBuffer.GetAddressOf());
      context->OMSetDepthStencilState(dsState.Get(), 0);
      context->RSSetState(rsState.Get());
      Scene.Render(context, view.viewProj, view.eye);

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

  const VehicleBlueprintManager& Simp::GetVehicleBlueprintManager()
  {
    assert(Instance);
    return Instance->VehicleBlueprintManager;
  }

  const SceneryBlueprintManager& Simp::GetSceneryBlueprintManager()
  {
    assert(Instance);
    return Instance->SceneryBlueprintManager;
  }

  const SplineBlueprintManager& Simp::GetSplineBlueprintManager()
  {
    assert(Instance);
    return Instance->SplineBlueprintManager;
  }

  const Graphics& Simp::GetGraphics()
  {
    assert(Instance);
    return Instance->Graphics;
  }

  const std::filesystem::path& Simp::GetOmsiPath()
  {
    assert(Instance);
    return Instance->OmsiPath;
  }

  Scene& Simp::GetScene()
  {
    assert(Instance);
    return Instance->Scene;
  }

  void Simp::OnClose(const WindowCloseEvent& e)
  {
    Running = false;
  }

  const Simp* Simp::Instance;

}
