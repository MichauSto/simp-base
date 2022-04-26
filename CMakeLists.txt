cmake_minimum_required(VERSION 3.22)

set(BUILD_SHARED_LIBS off)

add_subdirectory(dependencies/glm)
add_subdirectory(dependencies/fmt)
add_subdirectory(dependencies/entt)
add_subdirectory(dependencies/MikkTSpace)
add_subdirectory(dependencies/DirectXTex)

add_subdirectory(utils)
add_subdirectory(../simp_pluginhost/api simp_pluginhost_api)

set(OMSI_DIR_DEBUG "C:/Program Files (x86)/Steam/steamapps/common/OMSI 2" CACHE STRING "OMSI 2 Directory for Visual Studio debug sessions")

file(GLOB_RECURSE src source/*.cpp source/*.hpp)
file(GLOB_RECURSE api api/*.hpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/source PREFIX SIMP FILES ${src})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/api PREFIX API FILES ${api})

project(simp_api)
add_library(simp_api INTERFACE ${api})
set_property(TARGET simp_api PROPERTY CXX_STANDARD 20)
target_include_directories(simp_api INTERFACE api)


project(simp)
add_executable(simp ${src})
set_property(TARGET simp PROPERTY CXX_STANDARD 20)
target_link_libraries(simp PRIVATE simp_api simp_utils glm EnTT d3d12.lib dxgi.lib MikkTSpace DirectXTex simp_pluginhost_serverlib)
target_include_directories(simp PRIVATE source)

set_property(TARGET simp PROPERTY VS_DEBUGGER_COMMAND_ARGUMENTS "-OMSI \"${OMSI_DIR_DEBUG}\"")
set_property(TARGET simp PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:simp>")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT simp)

add_subdirectory(plugins/o3d)
add_subdirectory(plugins/aimesh)

add_custom_command(TARGET simp POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  ${CMAKE_CURRENT_SOURCE_DIR}/../simp_pluginhost/Host_$<CONFIG>/simp_pluginhost.exe
  $<TARGET_FILE_DIR:simp>/simp_pluginhost.exe)