#pragma once

#include <string>

namespace simp {

  struct LaunchSettings {
    int argCallback(const std::string* arr, int size);
    std::string omsiDir;
  };

  struct Simp {
    void run(const LaunchSettings& settings);
  };

}