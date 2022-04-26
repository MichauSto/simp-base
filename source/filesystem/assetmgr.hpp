#pragma once

#include <string>
#include <filesystem>
#include <memory>
#include <utils/dict.hpp>

namespace simp {

  template<typename T> 
  struct AssetManager {
  private:
    mutable Dict<std::weak_ptr<T>> Cache;
  protected:
    virtual std::shared_ptr<T> LoadAsset(const std::filesystem::path& path) const = 0;
  public:
    std::shared_ptr<T> Get(std::filesystem::path path) const {
      path = path.lexically_normal();
      auto& cache = Cache[path.generic_string()];
      auto ptr = cache.lock();
      if (!ptr) cache = ptr = LoadAsset(path);
      return ptr;
    }

  };

}