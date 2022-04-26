#include <Windows.h>

#include <vector>
#include <string>
#include <filesystem>
#include <functional>

#include "simp.hpp"
#include "filesystem/meshloader.hpp"

void parseArgs(LPWSTR cmdLine, std::function<int(const std::string*, int)> callback) {
  int numArgs;
  LPWSTR* szArgList = CommandLineToArgvW(cmdLine, &numArgs);

  std::vector<std::string> args{};
  args.reserve(numArgs);

  for (int i = 0; i < numArgs; ++i) {
    auto size = wcstombs(nullptr, szArgList[i], 0);
    auto& str = args.emplace_back(size, ' ');
    wcstombs(str.data(), szArgList[i], str.size());
  }

  LocalFree(szArgList);

  for (int i = 0; i < numArgs;) {
    i += callback(&args[i], numArgs - i);
  }

}

int WINAPI wWinMain(
  _In_ HINSTANCE hInstance,
  _In_opt_ HINSTANCE hPrevInstance,
  _In_ LPWSTR lpCmdLine,
  _In_ int nShowCmd) 
{

  simp::LaunchSettings settings{};
  parseArgs(lpCmdLine, std::bind(&simp::LaunchSettings::argCallback, &settings, std::placeholders::_1, std::placeholders::_2));
  auto currentPath = std::filesystem::current_path();

  simp::MeshLoader loader(settings.omsiDir);
  loader.LoadMesh("a", "a");
  return 0;
}
