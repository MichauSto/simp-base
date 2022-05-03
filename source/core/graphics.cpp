#include "graphics.hpp"

#include "core/window.hpp"
#include "core/event.hpp"

namespace simp {

  Graphics::Graphics()
    : Width(Window::getWidth()), Height(Window::getHeight())
  {
    HRESULT result;

    // Allocate swapchain description
    DXGI_SWAP_CHAIN_DESC swapchainDesc{};

    swapchainDesc.OutputWindow = Window::getHandle();

    swapchainDesc.BufferDesc.Width = Width;
    swapchainDesc.BufferDesc.Height = Height;
    swapchainDesc.BufferDesc.RefreshRate.Numerator = 0;
    swapchainDesc.BufferDesc.RefreshRate.Denominator = 0;
    swapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapchainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapchainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    swapchainDesc.SampleDesc.Count = 1;
    swapchainDesc.SampleDesc.Quality = 0;

    swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchainDesc.BufferCount = 3;
    swapchainDesc.Windowed = true;
    swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    // Required feature level
    D3D_FEATURE_LEVEL levels[] = {
      D3D_FEATURE_LEVEL_11_1
    };

    // Supported feature level returned by create function
    D3D_FEATURE_LEVEL featureLevel;

    // Create D3D11 device
    result = D3D11CreateDeviceAndSwapChain(
      nullptr,
      D3D_DRIVER_TYPE_HARDWARE,
      nullptr,
#ifndef NDEBUG
      D3D11_CREATE_DEVICE_DEBUG, // TODO set to 0 depending on release build
#else
      0,
#endif
      levels,
      ARRAYSIZE(levels),
      D3D11_SDK_VERSION,
      &swapchainDesc,
      &Swapchain,
      &Device,
      &featureLevel,
      &DeviceContext);
    if (FAILED(result)) {
      ExitProcess(result);
    }

    Device->QueryInterface<ID3D11Debug>(&Debug);

    // Create render target
    ComPtr<ID3D11Texture2D> pBackBuffer;
    Swapchain->GetBuffer(0, IID_PPV_ARGS(pBackBuffer.GetAddressOf()));
    Device->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &MainRenderTarget);
    pBackBuffer.Reset();

    // Create depth-stencil target
    ComPtr<ID3D11Texture2D> dsBuffer;
    D3D11_TEXTURE2D_DESC dsbDesc{};
    dsbDesc.Width = Width;
    dsbDesc.Height = Height;
    dsbDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsbDesc.MipLevels = 1;
    dsbDesc.ArraySize = 1;
    dsbDesc.SampleDesc.Count = 1;
    dsbDesc.SampleDesc.Quality = 0;
    dsbDesc.Usage = D3D11_USAGE_DEFAULT;
    dsbDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    dsbDesc.CPUAccessFlags = 0;
    dsbDesc.MiscFlags = 0;
    Device->CreateTexture2D(&dsbDesc, nullptr, &dsBuffer);
    Device->CreateDepthStencilView(dsBuffer.Get(), nullptr, &MainDepthStencil);

    Device->CreateDeferredContext(0, &DeferredContext);

    EventDispatcher::Get().sink<WindowResizeEvent>().connect<&Graphics::OnResize>(this);
  }

  ID3D11Device* Graphics::GetDevice() const
  {
    return Device.Get();
  }

  ID3D11DeviceContext* Graphics::GetDeviceContext() const
  {
    return DeferredContext.Get();
  }

  ID3D11RenderTargetView* Graphics::GetRenderTargetView() const
  {
    return MainRenderTarget.Get();
  }

  ID3D11DepthStencilView* Graphics::GetDepthStencilView() const
  {
    return MainDepthStencil.Get();
  }

  void Graphics::Present()
  {
    {
      ComPtr<ID3D11CommandList> cmd{};
      DeferredContext->FinishCommandList(false, &cmd);
      DeviceContext->ExecuteCommandList(cmd.Get(), false);
    }
    Swapchain->Present(1, 0);
  }

  void Graphics::OnResize(const WindowResizeEvent& e)
  {
    if (!Swapchain) return;

    if (Width == e.Width && Height == e.Height) return;
    Width = e.Width;
    Height = e.Height;

    MainRenderTarget.Reset();
    MainDepthStencil.Reset();

    Swapchain->ResizeBuffers(0, Width, Height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);
    // Create render target
    ComPtr<ID3D11Texture2D> pBackBuffer;
    Swapchain->GetBuffer(0, IID_PPV_ARGS(pBackBuffer.GetAddressOf()));
    Device->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &MainRenderTarget);

    ComPtr<ID3D11Texture2D> dsBuffer;
    D3D11_TEXTURE2D_DESC dsbDesc{};
    dsbDesc.Width = e.Width;
    dsbDesc.Height = e.Height;
    dsbDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsbDesc.MipLevels = 1;
    dsbDesc.ArraySize = 1;
    dsbDesc.SampleDesc.Count = 1;
    dsbDesc.SampleDesc.Quality = 0;
    dsbDesc.Usage = D3D11_USAGE_DEFAULT;
    dsbDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    dsbDesc.CPUAccessFlags = 0;
    dsbDesc.MiscFlags = 0;
    Device->CreateTexture2D(&dsbDesc, nullptr, &dsBuffer);
    Device->CreateDepthStencilView(dsBuffer.Get(), nullptr, &MainDepthStencil);

    pBackBuffer.Reset();
  }


}