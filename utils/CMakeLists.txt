
file(GLOB_RECURSE src "utils/*.hpp" "utils/*.h")

project(simp_utils VERSION 0.1.0)

add_library(simp_utils INTERFACE ${src})
target_link_libraries(simp_utils INTERFACE DirectXTex d3d11.lib dxgi.lib)
target_include_directories(simp_utils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
set_property(TARGET simp_utils PROPERTY CXX_STANDARD 20)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${src})
