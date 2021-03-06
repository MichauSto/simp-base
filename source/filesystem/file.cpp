#include "file.hpp"

#include <utils/utf8_check_is_valid.hpp>
#include <utils/CP1252_to_UTF8.hpp>
#include <fstream>
#include <cassert>

namespace simp {

  bool IsParentPath(const std::filesystem::path& lhs, const std::filesystem::path& rhs)
  {
    auto [begin, end] = std::mismatch(lhs.begin(), lhs.end(), rhs.begin());
    return begin == lhs.end();
  }

  std::string LoadFileText(const std::filesystem::path& path)
  {
    // TODO encoding conversion, we want UTF-8 everywhere...
    auto source = LoadFileBinary(path);
    if (source.empty()) return {};

    if (source.size() >= 2 && source[0] == '\xff' && source[1] == '\xfe') {
      source.push_back(0);
      source.push_back(0);
      size_t size = 0;
      auto e = wcstombs_s(
        &size, 
        nullptr, 
        0, 
        (const wchar_t*)(source.c_str() + 2), 
        0);
      assert(!e);
      std::string result(size - 1, ' ');
      wcstombs_s(
        nullptr, 
        result.data(), 
        size,
        (const wchar_t*)(source.c_str() + 2),
        _TRUNCATE);
      return result;
    }
    else if (utf8_check_is_valid(source.c_str())) {
      return source;
    }
    else {
      return CP1252_to_UTF8(source);
    }
  }

  std::string LoadFileBinary(const std::filesystem::path& path)
  {
    if (!std::filesystem::is_regular_file(path)) return {};
    std::ifstream ifs(path, std::ios::ate | std::ios::binary);
    size_t size = ifs.tellg();
    std::string buf(size, '\0');
    ifs.seekg(0);
    ifs.read(buf.data(), buf.size());
    return buf;
  }

}