#pragma once

#include <memory>
#include <string>
#include <utils/Windows.hpp>

namespace simp {

  struct Texture {
    static std::shared_ptr<Texture> LoadDDS(const std::string& data);
    static std::shared_ptr<Texture> LoadTGA(const std::string& data);
    static std::shared_ptr<Texture> LoadWIC(const std::string& data);
    Texture();
    Texture(const DirectX::ScratchImage& image);
    int s;
  };

}