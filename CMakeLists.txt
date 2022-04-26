cmake_minimum_required(VERSION 3.22)

add_subdirectory(dependencies/glm)
add_subdirectory(dependencies/entt)

set(OMSI_DIR_DEBUG "C:/Program Files (x86)/Steam/steamapps/common/OMSI 2" CACHE STRING "OMSI 2 Directory for Visual Studio debug sessions")

file(GLOB_RECURSE src source/*.cpp source/*.hpp)
file(GLOB_RECURSE api api/*.hpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/source PREFIX SIMP FILES ${src})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/api PREFIX API FILES ${api})

project(simp_api)
add_library(simp_api INTERFACE ${api})
target_include_directories(simp_api INTERFACE api)


project(simp)
add_executable(simp WIN32 ${src})
target_link_libraries(simp PRIVATE simp_api glm EnTT d3d12.lib dxgi.lib)

set_property(TARGET simp PROPERTY VS_DEBUGGER_COMMAND_ARGUMENTS "-OMSI \"${OMSI_DIR_DEBUG}\"")
set_property(TARGET simp PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:simp>")
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT simp)

add_subdirectory(plugins/o3d)
add_subdirectory(plugins/aimesh)