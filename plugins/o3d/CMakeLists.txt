project(simp_o3d)


file(GLOB_RECURSE src source/*.hpp source/*.cpp)

add_library(simp_o3d SHARED ${src})
target_link_libraries(simp_o3d PUBLIC simp_api glm)

add_custom_command(TARGET simp_o3d POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  $<TARGET_FILE:simp_o3d>
  $<TARGET_FILE_DIR:simp>/plugins/mesh/$<TARGET_FILE_NAME:simp_o3d>)