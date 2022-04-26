#pragma once

#include "pluginhost/server.hpp"

#include <filesystem>

namespace simp {

  struct PluginManagerLegacy {
    PluginManagerLegacy(const std::filesystem::path& omsiDir);
    PluginServer Server;
  };

}