cmake_minimum_required(VERSION 3.22)

add_compile_options(/MP /arch:AVX2)
add_compile_definitions($<$<CONFIG:checked,profile,release>:NDEBUG>)

set(OMSI_DIR_DEBUG "C:/Program Files (x86)/Steam/steamapps/common/OMSI 2" CACHE STRING "OMSI 2 Directory for Visual Studio debug sessions")
set(PLUGINHOST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../simp-pluginhost" CACHE STRING "Location of the legacy plugin host project")

set(BUILD_SHARED_LIBS off)
set(USE_MSVC_RUNTIME_LIBRARY_DLL on)

add_subdirectory(dependencies/glm)
add_subdirectory(dependencies/fmt)
add_subdirectory(dependencies/freetype)
add_subdirectory(dependencies/entt)
add_subdirectory(dependencies/MikkTSpace)
add_subdirectory(dependencies/DirectXTex)

add_subdirectory(physx)
add_subdirectory(utils)
add_subdirectory(shaders)
add_subdirectory(${PLUGINHOST_DIR}/api simp_pluginhost_api)


file(GLOB_RECURSE src source/*.cpp source/*.hpp)
file(GLOB_RECURSE api api/*.hpp)

# Imgui source
file(GLOB imgui_src 
 dependencies/imgui/*.cpp dependencies/imgui/*.h)

# Imgui Win32 backend
list(APPEND imgui_src 
  dependencies/imgui/backends/imgui_impl_dx11.h
  dependencies/imgui/backends/imgui_impl_dx11.cpp
  dependencies/imgui/backends/imgui_impl_win32.cpp
  dependencies/imgui/backends/imgui_impl_win32.h
  dependencies/imgui/misc/freetype/imgui_freetype.cpp
  dependencies/imgui/misc/freetype/imgui_freetype.h
  dependencies/imgui/misc/cpp/imgui_stdlib.cpp
  dependencies/imgui/misc/cpp/imgui_stdlib.h)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/source PREFIX SIMP FILES ${src})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/imgui PREFIX IMGUI FILES ${imgui_src})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/api PREFIX API FILES ${api})

project(simp_api)
add_library(simp_api INTERFACE ${api})
set_property(TARGET simp_api PROPERTY CXX_STANDARD 20)
target_include_directories(simp_api INTERFACE api)


project(simp)
add_executable(simp ${src} ${imgui_src})
set_property(TARGET simp PROPERTY CXX_STANDARD 20)
target_link_libraries(simp PRIVATE simp_api simp_utils glm EnTT dwmapi.lib d3d12.lib dxgi.lib MikkTSpace DirectXTex simp_pluginhost_serverlib simp_shaders freetype simp_physx)
target_include_directories(simp PRIVATE source dependencies/imgui dependencies/bullet3/src)
target_compile_definitions(simp PUBLIC GLM_FORCE_SWIZZLE GLM_FORCE_INTRINSICS)

set_property(TARGET simp PROPERTY VS_DEBUGGER_COMMAND_ARGUMENTS "-OMSI \"${OMSI_DIR_DEBUG}\"")
set_property(TARGET simp PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:simp>")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT simp)

add_subdirectory(plugins/o3d)
add_subdirectory(plugins/aimesh)

add_dependencies(simp simp_o3d simp_aimesh)

add_custom_command(TARGET simp POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  ${PLUGINHOST_DIR}/Host_$<CONFIG>/simp_pluginhost.exe
  $<TARGET_FILE_DIR:simp>/simp_pluginhost.exe)