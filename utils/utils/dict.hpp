#pragma once

#include <string>
#include <map>

namespace simp {

  struct DictComparer {
    using is_transparent = std::true_type;
    inline bool operator()(
      const std::string_view& lhs,
      const std::string_view& rhs) const {
      auto result = _strnicmp(lhs.data(), rhs.data(), std::min(lhs.length(), rhs.length()));
      if (!result) return lhs.length() < rhs.length();
      return result < 0;
    }
  };

  inline bool DictEqual(const std::string_view& lhs, const std::string_view& rhs) {
    if (lhs.length() != rhs.length()) return false;
    return !_strnicmp(lhs.data(), rhs.data(), lhs.length());
  }

  template<typename T>
  using Dict = std::map<std::string, T, DictComparer>;

}