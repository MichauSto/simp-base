file(GLOB_RECURSE src_vert *.vert.hlsl)
file(GLOB_RECURSE src_frag *.frag.hlsl)
file(GLOB_RECURSE src_inc *.cginc)

project(simp_shaders)

add_library(simp_shaders OBJECT ${src_vert} ${src_frag} ${src_inc}) 

set_target_properties(simp_shaders PROPERTIES LINKER_LANGUAGE HLSL)

set(SHADER_DIR "${CMAKE_BINARY_DIR}/prebuilt/\$(Configuration)")

target_include_directories(simp_shaders PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(simp_shaders INTERFACE ${SHADER_DIR})

set_property(SOURCE ${src_vert} ${src_frag} PROPERTY VS_SHADER_MODEL "5.0")
set_property(SOURCE ${src_vert} ${src_frag} PROPERTY VS_SHADER_OBJECT_FILE_NAME "")
set_property(SOURCE ${src_vert} ${src_frag} PROPERTY VS_SHADER_ENABLE_DEBUG $<$<NOT:$<CONFIG:release>>:true>)
set_property(SOURCE ${src_vert} ${src_frag} PROPERTY VS_SHADER_DISABLE_OPTIMIZATIONS $<$<NOT:$<CONFIG:release>>:true>)
foreach(src IN LISTS src_vert src_frag)
  get_filename_component(varname ${src} NAME_WLE)
  string(REGEX REPLACE "[^a-zA-Z0-9_]" "_" varname ${varname})
  set_property(SOURCE ${src} PROPERTY VS_SHADER_VARIABLE_NAME shader_${varname})
  message("${src} -> shader_${varname}")
endforeach()
set_property(SOURCE ${src_vert} ${src_frag} PROPERTY VS_SHADER_OUTPUT_HEADER_FILE "${SHADER_DIR}/shaders/%(Filename).hpp")
set_property(SOURCE ${src_vert} ${src_frag} PROPERTY VS_SHADER_FLAGS ${SHADER_DEFINE_FLAGS})

set_property(SOURCE ${src_vert} PROPERTY VS_SHADER_TYPE "Vertex")
set_property(SOURCE ${src_frag} PROPERTY VS_SHADER_TYPE "Pixel")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX Vertex FILES ${src_vert})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX Fragment FILES ${src_frag})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX Includes FILES ${src_inc})

set(USE_FOLDERS ON)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${src_vert} ${src_frag} ${src_inc})