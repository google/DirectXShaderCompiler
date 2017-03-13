//===-- Type.h - SPIR-V Type --*- C++-*------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_CLANG_SPIRV_TYPE_H
#define LLVM_CLANG_SPIRV_TYPE_H

#include <set>
#include <unordered_set>
#include <vector>

#include "clang/SPIRV/Decoration.h"
#include "clang/SPIRV/Option.h"
#include "clang/SPIRV/spirv.hpp"

namespace clang {
namespace spirv {

class SPIRVContext;

class Type {
public:
  spv::Op getOpcode() const { return opcode; }
  const std::vector<uint32_t> &getArgs() const { return args; }
  bool isBooleanType(const Type *t);
  bool isIntegerType(const Type *t);
  bool isFloatType(const Type *t);
  bool isNumericalType(const Type *t);
  bool isScalarType(const Type *t);
  bool isVectorType(const Type *t);
  bool isMatrixType(const Type *t);
  bool isArrayType(const Type *t);
  bool isStructureType(const Type *t);
  bool isAggregateType(const Type *t);
  bool isCompositeType(const Type *t);
  bool isImageType(const Type *t);

  static const Type *getType(SPIRVContext &ctx, spv::Op op,
                             std::vector<uint32_t> arg = {},
                             std::set<const Decoration *> decs = {});

  static const Type *getTypeVoid(SPIRVContext &ctx);
  static const Type *getTypeBool(SPIRVContext &ctx);
  static const Type *getTypeInt8(SPIRVContext &ctx);
  static const Type *getTypeUnsignedInt8(SPIRVContext &ctx);
  static const Type *getTypeInt16(SPIRVContext &ctx);
  static const Type *getTypeUnsignedInt16(SPIRVContext &ctx);
  static const Type *getTypeInt32(SPIRVContext &ctx);
  static const Type *getTypeUnsignedInt32(SPIRVContext &ctx);
  static const Type *getTypeInt64(SPIRVContext &ctx);
  static const Type *getTypeUnsignedInt64(SPIRVContext &ctx);
  static const Type *getTypeFloat16(SPIRVContext &ctx);
  static const Type *getTypeFloat32(SPIRVContext &ctx);
  static const Type *getTypeFloat64(SPIRVContext &ctx);
  static const Type *getTypeVector(SPIRVContext &ctx, uint32_t component_type,
                                   uint32_t vec_size);
  static const Type *getTypeVec2(SPIRVContext &ctx, uint32_t component_type);
  static const Type *getTypeVec3(SPIRVContext &ctx, uint32_t component_type);
  static const Type *getTypeVec4(SPIRVContext &ctx, uint32_t component_type);
  static const Type *getTypeMatrix(SPIRVContext &ctx, uint32_t column_type_id,
                                   uint32_t column_count);
  static const Type *
  getTypeImage(SPIRVContext &ctx, uint32_t sampled_type, spv::Dim dim,
               uint32_t depth, uint32_t arrayed, uint32_t ms, uint32_t sampled,
               spv::ImageFormat image_format,
               Option<spv::AccessQualifier> access_qualifier);
  static const Type *getTypeSampler(SPIRVContext &ctx);
  static const Type *getTypeSampledImage(SPIRVContext &ctx,
                                         uint32_t imag_type_id);
  static const Type *getTypeArray(SPIRVContext &ctx, uint32_t component_type_id,
                                  uint32_t len_id);
  static const Type *getTypeRuntimeArray(SPIRVContext &ctx,
                                         uint32_t component_type_id);
  static const Type *getTypeStruct(SPIRVContext &ctx,
                                   std::initializer_list<uint32_t> members);
  static const Type *getTypeOpaque(SPIRVContext &ctx, std::string name);
  static const Type *getTyePointer(SPIRVContext &ctx,
                                   spv::StorageClass storage_class,
                                   uint32_t type);
  static const Type *getTypeFunction(SPIRVContext &ctx, uint32_t return_type,
                                     std::initializer_list<uint32_t> params);
  static const Type *getTypeEvent(SPIRVContext &ctx);
  static const Type *getTypeDeviceEvent(SPIRVContext &ctx);
  static const Type *getTypeQueue(SPIRVContext &ctx);
  static const Type *getTypePipe(SPIRVContext &ctx,
                                 spv::AccessQualifier qualifier);
  static const Type *getTypeForwardPointer(SPIRVContext &ctx,
                                           uint32_t pointer_type,
                                           spv::StorageClass storage_class);
  static const Type *getTypePipeStorage(SPIRVContext &ctx);
  static const Type *getTypeNamedBarrier(SPIRVContext &ctx);

  bool operator==(const Type &other) const {
    return opcode == other.opcode && args == other.args &&
           decorations == other.decorations;
  }

private:
  Type(spv::Op op, std::vector<uint32_t> arg = {},
       std::set<const Decoration *> dec = {});

  // Uses ExistingTypes to return an unique type.
  static const Type *getUniqueType(SPIRVContext &context, Type &t);

  // Private members that define a unique SPIR-V type.
  spv::Op opcode;
  std::vector<uint32_t> args;
  std::set<const Decoration *> decorations;
};

} // end namespace spirv
} // end namespace clang

#endif
