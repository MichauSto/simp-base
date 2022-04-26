
#include "legacy.hpp"

namespace simp {



  PluginManagerLegacy::PluginManagerLegacy(const std::filesystem::path& omsiDir)
    : Server("simp_pluginhost.exe", omsiDir)
  {
  }


}