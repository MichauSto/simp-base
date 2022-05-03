#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include <DirectXTex.h>
#include <d3d11.h>
#include <dxgi.h>
#include <wrl.h>

#undef NOMINMAX
#undef WIN32_LEAN_AND_MEAN

#include <string>
#include <cctype>
#include <algorithm>

namespace simp {
  template<typename T>
  using ComPtr = Microsoft::WRL::ComPtr<T>;

  inline std::string HrTranslate(const HRESULT result) {
    LPSTR buf = nullptr;
    auto size = FormatMessageA(
      FORMAT_MESSAGE_ALLOCATE_BUFFER |
      FORMAT_MESSAGE_FROM_SYSTEM,
      nullptr,
      result,
      0,
      (LPSTR)&buf,
      0,
      nullptr);
    std::string_view sv((const char*)buf, (const char*)buf + size);
    std::string output(
      sv.begin(), 
      std::find_if<std::string_view::const_reverse_iterator, int(__cdecl *)(int)>(
        sv.rbegin(), 
        sv.rend(),
        std::isprint).base());
    LocalFree(buf);
    return output;
  }
}