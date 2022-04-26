#include "texture.hpp"

namespace simp {

  Texture::Texture() {
    static int counter = 0;
    s = counter++;
  }

  Texture::Texture(const DirectX::ScratchImage& image)
  {
    auto metadata = image.GetMetadata();
    return;
    //DirectX::CreateShaderResourceView(Simp::GetGraphics().GetDevice(), image.GetImages(), image.GetImageCount(), image.GetMetadata(), )
  }

  std::shared_ptr<Texture> Texture::LoadDDS(const std::string& data)
  {
    DirectX::ScratchImage image;
    auto result = DirectX::LoadFromDDSMemory(data.data(), data.size(), DirectX::DDS_FLAGS_NONE, nullptr, image);
    return SUCCEEDED(result) ? std::make_shared<Texture>(image) : nullptr;
  }

  std::shared_ptr<Texture> Texture::LoadTGA(const std::string& data)
  {
    DirectX::ScratchImage image;
    DirectX::ScratchImage mipChain;
    auto result = DirectX::LoadFromTGAMemory(data.data(), data.size(), DirectX::TGA_FLAGS_NONE, nullptr, image);
    DirectX::GenerateMipMaps(image.GetImages()[0], DirectX::TEX_FILTER_DEFAULT, 0, mipChain);
    return SUCCEEDED(result) ? std::make_shared<Texture>(mipChain) : nullptr;
  }

  std::shared_ptr<Texture> Texture::LoadWIC(const std::string& data)
  {
    DirectX::ScratchImage image;
    DirectX::ScratchImage mipChain;
    auto result = DirectX::LoadFromWICMemory(data.data(), data.size(), DirectX::WIC_FLAGS_NONE, nullptr, image);
    DirectX::GenerateMipMaps(image.GetImages()[0], DirectX::TEX_FILTER_DEFAULT, 0, mipChain);
    return SUCCEEDED(result) ? std::make_shared<Texture>(mipChain) : nullptr;
  }

}