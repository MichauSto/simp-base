#pragma once

#include "core/event.hpp"

#include <utils/Windows.hpp>

namespace simp {

  struct Graphics {

    Graphics();

    ID3D11Device* GetDevice() const;
    ID3D11DeviceContext* GetDeviceContext() const;
    ID3D11DeviceContext* GetImmediateContext() const;
    ID3D11RenderTargetView* GetRenderTargetView() const;
    ID3D11DepthStencilView* GetDepthStencilView() const;
    void Present();
  private:
    uint32_t Width;
    uint32_t Height;
    void OnResize(const WindowResizeEvent& e);
    ComPtr<ID3D11Device> Device;
    ComPtr<ID3D11DeviceContext> DeferredContext;
    ComPtr<ID3D11DeviceContext> DeviceContext;
    ComPtr<IDXGISwapChain> Swapchain;
    ComPtr<ID3D11RenderTargetView> MainRenderTarget;
    ComPtr<ID3D11DepthStencilView> MainDepthStencil;
    ComPtr<ID3D11Debug> Debug;
  };

}