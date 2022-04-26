project(simp_aimesh)

set(BUILD_SHARED_LIBS OFF)
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_X_IMPORTER ON)
set(ASSIMP_BUILD_3DS_IMPORTER ON)
set(ASSIMP_BUILD_FBX_IMPORTER ON)
set(ASSIMP_BUILD_OBJ_IMPORTER ON)
set(ASSIMP_BUILD_DAE_IMPORTER ON)
add_subdirectory(assimp)

file(GLOB_RECURSE src source/*.hpp source/*.cpp)

add_library(simp_aimesh SHARED ${src})
target_link_libraries(simp_aimesh PUBLIC assimp simp_api glm)

add_custom_command(TARGET simp_aimesh POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  $<TARGET_FILE:simp_aimesh>
  $<TARGET_FILE_DIR:simp>/plugins/mesh/$<TARGET_FILE_NAME:simp_aimesh>)