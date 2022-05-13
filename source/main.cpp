#include <Windows.h>

#include <vector>
#include <string>
#include <filesystem>
#include <functional>
#include <fstream>

#include <filesystem/file.hpp>
#include <filesystem/cfgfile.hpp>
#include <blueprints/vehicleblueprint.hpp>

#include "scene/global.hpp"

#include "simp.hpp"
#include "visual/shaders.hpp"
#include "core/event.hpp"
#include "core/window.hpp"
#include "core/winmsgtranslator.hpp"
#include "filesystem/meshloader.hpp"

#include "plugin/legacy.hpp"

#include <glm/gtx/transform.hpp>

#if 0
#include "scene/rendercomponents.hpp"
void bFlTest() {
  using namespace simp;
  auto positive = RenderOrderComponent::CalcDistanceUnsigned(1.f);
  auto zero = RenderOrderComponent::CalcDistanceUnsigned(0.f);
  auto negZero = RenderOrderComponent::CalcDistanceUnsigned(-0.f);
  auto negative = RenderOrderComponent::CalcDistanceUnsigned(-1.f);
  assert(zero == negZero);
  assert(positive < zero);
  assert(negative > zero);
}
#endif

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
#ifdef _SIMP_WINAPP
#define _SIMP_MAIN_ARGS lpCmdLine
#else
#define _SIMP_MAIN_ARGS argc, argv
#endif
  using namespace simp;

  std::setlocale(LC_ALL, ".UTF8");
  CoInitialize(nullptr);

  LaunchSettings settings{};

  parseArgs(_SIMP_MAIN_ARGS, std::bind(&LaunchSettings::argCallback, &settings, std::placeholders::_1, std::placeholders::_2));

  EventDispatcher dispatcher{};
  WinMsgTranslator translator{};
  Window window{};
  Simp simp{ settings };
  Shaders shaders{};
  //Window::setFullscreen(true);

  PluginManagerLegacy mgr(settings.omsiDir);

  Global global("maps/London/global.cfg");

  const auto& entryPoint = global.EntryPoints[2];
  auto [tid, obj] = global.ObjectLookup[entryPoint.Object];

  const auto& tile = global.Tiles[tid];
  auto position = tile.Scenery[obj].Position;

  VehicleBlueprint bp("Vehicles/MB_O407_93/MB_O407_neu.bus");
  bp.Instantiate(simp.GetScene(), position, tile.Co);
  global.Instantiate(simp.GetScene(), tile.Co, 4);
  
  //ModelBlueprint bp(
  //  LoadFileText(settings.omsiDir / "Vehicles/MAN_SD200/Model/model_SD77.cfg"),
  //  "Vehicles/MAN_SD200/Model", 
  //  "Vehicles/MAN_SD200/Texture",
  //  {},
  //  {});

  simp.Run(position, tile.Co);

  CoUninitialize();
  return 0;
}
