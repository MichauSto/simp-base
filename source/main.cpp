#include <Windows.h>

#include <vector>
#include <string>
#include <filesystem>
#include <functional>
#include <fstream>

#include <filesystem/file.hpp>
#include <filesystem/cfgfile.hpp>
#include <blueprints/modelblueprint.hpp>

#include "simp.hpp"
#include "filesystem/meshloader.hpp"

#include "plugin/legacy.hpp"

#ifdef _SIMP_WINAPP
void parseArgs(LPWSTR cmdLine, std::function<int(const std::string*, int)> callback) {
  int numArgs;
  LPWSTR* szArgList = CommandLineToArgvW(cmdLine, &numArgs);

  std::vector<std::string> args{};
  args.reserve(numArgs);

  for (int i = 0; i < numArgs; ++i) {
    size_t size;
    wcstombs_s(&size, nullptr, 0, szArgList[i], 0);
    auto& str = args.emplace_back(size - 1, ' ');
    wcstombs_s(nullptr, str.data(), size, szArgList[i], _TRUNCATE);
  }

  LocalFree(szArgList);

  for (int i = 0; i < numArgs;) {
    i += callback(&args[i], numArgs - i);
  }

}
#else
void parseArgs(int argc, const wchar_t** argv, std::function<int(const std::string*, int)> callback) {
  std::vector<std::string> args{};
  args.reserve(argc);

  for (int i = 0; i < argc; ++i) {
    size_t size;
    wcstombs_s(&size, nullptr, 0, argv[i], 0);
    auto& str = args.emplace_back(size - 1, ' ');
    wcstombs_s(nullptr, str.data(), size, argv[i], _TRUNCATE);
  }

  for (int i = 0; i < argc;) {
    i += callback(&args[i], argc - i);
  }
}
#endif

#ifdef _SIMP_WINAPP
int WINAPI wWinMain(
  _In_ HINSTANCE hInstance,
  _In_opt_ HINSTANCE hPrevInstance,
  _In_ LPWSTR lpCmdLine,
  _In_ int nShowCmd) 
#else
int wmain(int argc, const wchar_t** argv)
#endif
{
  using namespace simp;

  std::setlocale(LC_ALL, ".UTF8");
  CoInitialize(nullptr);

  LaunchSettings settings{};

#ifdef _SIMP_WINAPP
  parseArgs(lpCmdLine, std::bind(&LaunchSettings::argCallback, &settings, std::placeholders::_1, std::placeholders::_2));
#else
  parseArgs(argc, argv, std::bind(&LaunchSettings::argCallback, &settings, std::placeholders::_1, std::placeholders::_2));
#endif

  Simp simp{ settings };

  PluginManagerLegacy mgr(settings.omsiDir);
  
  ModelBlueprint bp(
    LoadFileText(settings.omsiDir / "Vehicles/MAN_SD200/Model/model_SD77.cfg"),
    "Vehicles/MAN_SD200/Model", 
    "Vehicles/MAN_SD200/Texture",
    {},
    {});

  CoUninitialize();
  return 0;
}
