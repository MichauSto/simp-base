#pragma once

#include <cstdint>

namespace simp {

  namespace MeshLoaderApi {

    typedef void* PMeshInstance;

    struct IMeshLoader {
      virtual PMeshInstance __stdcall LoadMesh(
        uint64_t size, 
        const char* data, 
        const char* filename) const = 0;
      virtual void __stdcall FreeMesh(
        PMeshInstance instance) const = 0;
      virtual uint32_t __stdcall GetMaterialCount(
        PMeshInstance instance) const = 0;
      virtual const char* __stdcall GetMaterialName(
        PMeshInstance instance,
        uint32_t materialId) const = 0;
      virtual void __stdcall GetMaterialParams(
        PMeshInstance instance,
        uint32_t materialId,
        float diffuse[],
        float ambient[],
        float specular[],
        float emit[],
        float& specularExp,
        float& alpha) const = 0;
      virtual uint32_t __stdcall GetTriangleCount(
        PMeshInstance instance, 
        uint32_t materialId) const = 0;
      virtual void __stdcall GetVertexPosition(
        PMeshInstance instance,
        uint32_t materialId,
        uint32_t triangleId,
        uint32_t vertexId,
        float position[]) const = 0;
      virtual void __stdcall GetVertexNormal(
        PMeshInstance instance,
        uint32_t materialId,
        uint32_t triangleId,
        uint32_t vertexId,
        float normal[]) const = 0;
      virtual void __stdcall GetVertexTexCoord(
        PMeshInstance instance,
        uint32_t materialId,
        uint32_t triangleId,
        uint32_t vertexId,
        float texCoord[]) const = 0;
      virtual void __stdcall GetVertexIndices(
        PMeshInstance instance,
        uint32_t materialId,
        uint32_t triangleId,
        uint32_t vertexId,
        uint32_t bones[]) const = 0;
      virtual void __stdcall GetVertexWeights(
        PMeshInstance instance,
        uint32_t materialId,
        uint32_t triangleId,
        uint32_t vertexId,
        float weights[]) const = 0;
      virtual uint32_t __stdcall GetBoneCount(
        PMeshInstance instance) const = 0;
      virtual const char* __stdcall GetBoneName(
        PMeshInstance instance,
        uint32_t boneId) const = 0;
      virtual void __stdcall GetOriginTransform(
        PMeshInstance instance,
        float matrix[]) const = 0;
      virtual ~IMeshLoader() = default;
    };

  }

}