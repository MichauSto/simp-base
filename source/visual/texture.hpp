#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <utils/Windows.hpp>

#include <condition_variable>
#include <thread>

//#include <thread_pool.hpp>

namespace simp {

  struct Texture {
    static std::shared_ptr<Texture> LoadDDS(const std::filesystem::path& path);
    static std::shared_ptr<Texture> LoadTGA(const std::filesystem::path& path);
    static std::shared_ptr<Texture> LoadWIC(const std::filesystem::path& path);
    Texture() = default;
    ComPtr<ID3D11ShaderResourceView> GetTextureView();
  private:
    //static thread_pool& GetThreadPool();
    static ComPtr<ID3D11ShaderResourceView> FromDDS(const std::string& data);
    static ComPtr<ID3D11ShaderResourceView> FromTGA(const std::string& data);
    static ComPtr<ID3D11ShaderResourceView> FromWIC(const std::string& data);
    static ComPtr<ID3D11ShaderResourceView> CreateResource(const DirectX::ScratchImage& image);
    mutable std::mutex TextureReadyMutex;
    mutable std::condition_variable CvTextureReady;
    bool IsReady = false;
    //void NotifyReady();
    //std::jthread loadThread;
    ComPtr<ID3D11ShaderResourceView> TextureView{};
    //std::future<ComPtr<ID3D11ShaderResourceView>> TextureViewFuture{};
  };

}