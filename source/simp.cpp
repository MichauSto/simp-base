#include "simp.hpp"

using namespace simp;

int LaunchSettings::argCallback(const std::string* arr, int size)
{
  if (!_stricmp(arr[0].c_str(), "-omsi")) {
    omsiDir = arr[1];
    return 2;
  }
  return 1;
}
