#include "texture.hpp"
#include "simp.hpp"
#include "filesystem/file.hpp"

namespace simp {

  ComPtr<ID3D11ShaderResourceView> Texture::GetTextureView()
  {
    //if (TextureViewFuture.valid()) TextureView = TextureViewFuture.get();
    return TextureView;
  }

  //thread_pool& Texture::GetThreadPool()
  //{
  //  static thread_pool pool{};
  //  return pool;
  //}

  ComPtr<ID3D11ShaderResourceView> Texture::CreateResource(const DirectX::ScratchImage& image)
  {
    ComPtr<ID3D11ShaderResourceView> view{};
    auto result = DirectX::CreateShaderResourceView(
      Simp::GetGraphics().GetDevice(), 
      image.GetImages(), 
      image.GetImageCount(), 
      image.GetMetadata(), 
      &view);
    assert(SUCCEEDED(result));
    return view;
  }

  //void Texture::NotifyReady()
  //{
  //  IsReady = true;
  //  CvTextureReady.notify_all();
  //}

  std::shared_ptr<Texture> Texture::LoadDDS(const std::filesystem::path& path)
  {
    if (!std::filesystem::is_regular_file(path))
      return nullptr;
    auto texture = std::make_shared<Texture>();
    texture->TextureView = FromDDS(LoadFileBinary(path));
    //texture->TextureViewFuture = GetThreadPool().submit(
    //  [](const std::filesystem::path& path) -> ComPtr<ID3D11ShaderResourceView> {
    //    if (!std::filesystem::exists(path)) return nullptr;
    //    return FromDDS(LoadFileBinary(path));
    //  }, 
    //  path);
    return texture;
  }

  std::shared_ptr<Texture> Texture::LoadTGA(const std::filesystem::path& path)
  {
    if (!std::filesystem::is_regular_file(path))
      return nullptr;
    auto texture = std::make_shared<Texture>();
    texture->TextureView = FromTGA(LoadFileBinary(path));
    //texture->TextureViewFuture = GetThreadPool().submit(
    //  [](const std::filesystem::path& path) -> ComPtr<ID3D11ShaderResourceView> {
    //    if (!std::filesystem::exists(path)) return nullptr;
    //    return FromTGA(LoadFileBinary(path));
    //  },
    //  path);
    return texture;
  }

  std::shared_ptr<Texture> Texture::LoadWIC(const std::filesystem::path& path)
  {
    if (!std::filesystem::is_regular_file(path))
      return nullptr;
    auto texture = std::make_shared<Texture>();
    texture->TextureView = FromWIC(LoadFileBinary(path));
    //texture->TextureViewFuture = GetThreadPool().submit(
    //  [](const std::filesystem::path& path) -> ComPtr<ID3D11ShaderResourceView> {
    //    if (!std::filesystem::exists(path)) return nullptr;
    //    return FromWIC(LoadFileBinary(path));
    //  },
    //  path);
    return texture;
  }

  ComPtr<ID3D11ShaderResourceView> Texture::FromTGA(const std::string& data)
  {
    DirectX::ScratchImage image;
    DirectX::ScratchImage mipChain;
    auto result = DirectX::LoadFromTGAMemory(data.data(), data.size(), DirectX::TGA_FLAGS_NONE, nullptr, image);
    if (image.GetMetadata().width == 1 && image.GetMetadata().height == 1) {
      return CreateResource(image);
    }
    result = DirectX::GenerateMipMaps(image.GetImages()[0], DirectX::TEX_FILTER_SEPARATE_ALPHA, 0, mipChain);
    return CreateResource(mipChain);
  }

  ComPtr<ID3D11ShaderResourceView> Texture::FromWIC(const std::string& data)
  {
    DirectX::ScratchImage image;
    DirectX::ScratchImage mipChain;
    auto result = DirectX::LoadFromWICMemory(data.data(), data.size(), DirectX::WIC_FLAGS_NONE, nullptr, image);
    assert(SUCCEEDED(result));
    if (image.GetMetadata().width == 1 && image.GetMetadata().height == 1) {
      return CreateResource(image);
    }
    result = DirectX::GenerateMipMaps(image.GetImages()[0], DirectX::TEX_FILTER_SEPARATE_ALPHA, 0, mipChain);
    assert(SUCCEEDED(result));
    return CreateResource(mipChain);
  }

  ComPtr<ID3D11ShaderResourceView> Texture::FromDDS(const std::string& data)
  {
    DirectX::ScratchImage image;
    auto result = DirectX::LoadFromDDSMemory(data.data(), data.size(), DirectX::DDS_FLAGS_NONE, nullptr, image);
    if (FAILED(result)) {
      return FromWIC(data);
    }
    auto meta = image.GetMetadata();
    if (DirectX::IsCompressed(meta.format) && (meta.width % 4 || meta.height % 4)) {
      DirectX::ScratchImage decompressed{};
      result = DirectX::Decompress(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DXGI_FORMAT_UNKNOWN, decompressed);
      assert(SUCCEEDED(result));
      if (meta.mipLevels > 1 || (meta.width == 1 && meta.height == 1)) {
        return CreateResource(decompressed);
      }
      else {
        DirectX::ScratchImage mipChain{};
        result = DirectX::GenerateMipMaps(decompressed.GetImages()[0], DirectX::TEX_FILTER_SEPARATE_ALPHA, 0, mipChain);
        assert(SUCCEEDED(result));
        return CreateResource(mipChain);
      }
    }
    else {
      if (meta.mipLevels > 1 || (meta.width == 1 && meta.height == 1)) {
        return CreateResource(image);
      }
      else {
        if (DirectX::IsCompressed(meta.format))
        {
          DirectX::ScratchImage decompressed{};
          result = DirectX::Decompress(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DXGI_FORMAT_UNKNOWN, decompressed);
          assert(SUCCEEDED(result));
          std::swap(decompressed, image);
        }
        DirectX::ScratchImage mipChain{};
        result = DirectX::GenerateMipMaps(image.GetImages()[0], DirectX::TEX_FILTER_SEPARATE_ALPHA, 0, mipChain);
        assert(SUCCEEDED(result));
        return CreateResource(mipChain);
      }
    }
  }

}