#pragma once

#include <string>
#include <filesystem>

namespace simp {

  bool IsParentPath(const std::filesystem::path& lhs, const std::filesystem::path& rhs);
  std::string LoadFileText(const std::filesystem::path& path);
  std::string LoadFileBinary(const std::filesystem::path& path);

}