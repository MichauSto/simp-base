#include "rendercomponents.hpp"
#include "transformcomponents.hpp"
#include "materialcomponents.hpp"

#include "visual/shaders.hpp"
#include "scene.hpp"
#include "simp.hpp"

#include <map>

namespace simp {

  void Scene::Render(ID3D11DeviceContext* context, const glm::mat4& viewMatrix, const glm::vec3& eye)
  {
    {
      auto view = Registry.view<TransformComponent, RenderBoxComponent>();
      for (auto e : view) {
        auto [transform, box] = view.get(e);
        glm::vec4 cs = viewMatrix * transform.WorldTransform * glm::vec4(0.f, 0.f, 0.f, 1.f);

        {
          auto inverseTransform = glm::inverse(transform.WorldTransform);
          glm::vec3 eyeLocal = inverseTransform * glm::vec4(eye, 1.f);
          auto pos = glm::max(glm::vec3{ 0.f }, glm::abs(eyeLocal - box.Size / 2.f));
          box.Distance = glm::dot(pos, pos);
        }

        if (cs.w)
        {
          //box.Distance = cs.z / cs.w;
          box.ScreenSize = 2.f * box.Radius / cs.w;
        }
        else
        {
          //box.Distance = 0.f;
          box.ScreenSize = std::numeric_limits<float>::max();
        }

      }
    }

    {
      auto view = Registry.view<RenderDistanceComponent, RenderOrderComponent>();
      for (auto e : view) {
        auto [dist, order] = view.get(e);
        const auto& box = Registry.get<RenderBoxComponent>(dist.RefObject);
        order.Parameters.Distance = RenderOrderComponent::CalcDistanceUnsigned(box.Distance);
      }
    }

    Registry.clear<RenderFlagComponent>();

    {
      auto view = Registry.view<LodComponent>(entt::exclude<RenderFlagComponent>);
      for (auto e : view) {
        auto [lod] = view.get(e);
        auto rb = Registry.get<RenderBoxComponent>(lod.RefObject);
        if (lod.MaxSize < rb.ScreenSize || lod.MinSize >= rb.ScreenSize) {
          Registry.emplace<RenderFlagComponent>(e);
        }
      }
    }

    {
      auto view = Registry.view<VisibleComponent>(entt::exclude<RenderFlagComponent>);
      for (auto e : view) {
        auto [visible] = view.get(e);
        if (!visible.Visible) {
          Registry.emplace<RenderFlagComponent>(e);
        }
      }
    }

    {
      auto view = Registry.view<ViewpointComponent>(entt::exclude<RenderFlagComponent>);
      for (auto e : view) {
        auto [vp] = view.get(e);
        if (!((vp.Mask & 1))) {
          Registry.emplace<RenderFlagComponent>(e);
        }
      }
    }

    auto group = Registry.group<RenderComponent, RenderOrderComponent, MaterialComponent>(entt::exclude<RenderFlagComponent>);

    group.sort<RenderOrderComponent>([](const auto& lhs, const auto& rhs) -> bool { return lhs.Order < rhs.Order; });

    for (auto e : group) {
      auto [render, order, material] = group.get(e);
      assert(render.m_ModelBuffer);
      const auto& model = render.m_Mesh->Models[render.m_Index];
      context->VSSetShader(Shaders::GetVertexStatic(), nullptr, 0);
      context->PSSetShader(Shaders::GetPixelLegacy(), nullptr, 0);
      context->IASetInputLayout(Shaders::GetInputLayoutStatic());
      context->IASetVertexBuffers(
        0,
        1,
        render.m_Mesh->VertexBuffer.GetAddressOf(),
        Shaders::GetStridesStatic(),
        Shaders::GetOffsetsStatic());
      context->IASetIndexBuffer(
        render.m_Mesh->IndexBuffer.Get(),
        DXGI_FORMAT_R16_UINT,
        0);

      context->VSSetConstantBuffers(1, 1, render.m_ModelBuffer.GetAddressOf());

      material.m_Material.Use(context);

      context->DrawIndexed(model.IndexCount, model.IndexOffset, 0);
    }
  }

  void Scene::UpdateRenderers(ID3D11DeviceContext* context)
  {
    auto view = Registry.view<TransformDirtyComponent, TransformComponent, RenderComponent>();
    for (auto e : view) {
      auto [dirty, transform, render] = view.get(e);
      D3D11_MAPPED_SUBRESOURCE msrModel{};
      context->Map(render.m_ModelBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msrModel);

      auto& model = *static_cast<glm::mat4*>(msrModel.pData);
      model = transform.WorldTransform;

      context->Unmap(render.m_ModelBuffer.Get(), 0);
    }
  }

  RenderOrderComponent::RenderOrderComponent(uint8_t _renderType, float _distance, uint32_t _ordinal)
  {
    Parameters.RenderType = _renderType;
    Parameters.Distance = CalcDistanceUnsigned(_distance);
    Parameters.Mjr = _ordinal >> 16;
    Parameters.Mnr = _ordinal;
  }

  uint32_t RenderOrderComponent::CalcDistanceUnsigned(const float& _dist)
  {
    uint32_t bin = reinterpret_cast<const uint32_t&>(_dist);
    constexpr static uint32_t signMask = 1 << 31;
    return (bin & signMask) ? bin : (signMask - bin);
  }

  RenderComponent::RenderComponent(
    const std::shared_ptr<Mesh>& _mesh,
    int _index) 
    :m_Mesh(_mesh), m_Index(_index), m_ModelBuffer()
  {
    D3D11_BUFFER_DESC mbDesc{};
    mbDesc.ByteWidth = sizeof(glm::mat4);
    mbDesc.Usage = D3D11_USAGE_DYNAMIC;
    mbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    mbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    mbDesc.MiscFlags = 0;
    mbDesc.StructureByteStride = 0;
    
    Simp::GetGraphics().GetDevice()->CreateBuffer(&mbDesc, nullptr, &m_ModelBuffer);
  }


  //RenderComponent::RenderComponent(
  //  const std::shared_ptr<Mesh>& _mesh, 
  //  int _index,
  //  const std::shared_ptr<Texture>& _texture,
  //  bool _alphaBlend,
  //  D3D11_TEXTURE_ADDRESS_MODE _texAdressMode,
  //  glm::vec4 _borderColor)
  //  : m_Mesh(_mesh), m_Index(_index), m_Texture(_texture), m_AlphaBlend(_alphaBlend), m_ModelBuffer()
  //{
  //  D3D11_BUFFER_DESC mbDesc{};
  //  mbDesc.ByteWidth = sizeof(glm::mat4);
  //  mbDesc.Usage = D3D11_USAGE_DYNAMIC;
  //  mbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  //  mbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  //  mbDesc.MiscFlags = 0;
  //  mbDesc.StructureByteStride = 0;
  //
  //  Simp::GetGraphics().GetDevice()->CreateBuffer(&mbDesc, nullptr, &m_ModelBuffer);
  //
  //
  //  D3D11_SAMPLER_DESC msDesc{};
  //
  //  msDesc.Filter = D3D11_FILTER_ANISOTROPIC;
  //  msDesc.AddressU = _texAdressMode;
  //  msDesc.AddressV = _texAdressMode;
  //  msDesc.AddressW = _texAdressMode;
  //  msDesc.MipLODBias = 0.f;
  //  msDesc.MaxAnisotropy = 16;
  //  *reinterpret_cast<glm::vec4*>(msDesc.BorderColor) = _borderColor;
  //
  //  Simp::GetGraphics().GetDevice()->CreateSamplerState(&msDesc, &m_MainSamplerState);
  //}


  RenderBoxComponent::RenderBoxComponent(const glm::vec3& _size)
    : Size(_size), Radius(.5f * glm::length(_size))
  {
  }


}