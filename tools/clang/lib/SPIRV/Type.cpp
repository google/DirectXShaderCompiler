//===--- Type.cpp - SPIR-V Type implementation-----------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "clang/SPIRV/Type.h"
#include "clang/SPIRV/SPIRVContext.h"
#include "clang/SPIRV/Utils.h"
#include "clang/SPIRV/spirv.hpp"
#include "llvm/llvm_assert/assert.h"

namespace clang {
namespace spirv {

Type::Type(spv::Op op, std::vector<uint32_t> arg,
           std::set<const Decoration *> decs)
    : opcode(op), args(arg), decorations(decs) {}

const Type *Type::getUniqueType(SPIRVContext &context, Type &t) {
  // Insert function will only insert if it doesn't already exist in the set.
  context.ExistingTypes.insert(t);
  return &(*context.ExistingTypes.find(t));
}
const Type *Type::getTypeVoid(SPIRVContext &context) {
  Type t = Type(spv::Op::OpTypeVoid);
  return getUniqueType(context, t);
}
const Type *Type::getTypeBool(SPIRVContext &context) {
  Type t = Type(spv::Op::OpTypeBool);
  return getUniqueType(context, t);
}
const Type *Type::getTypeInt8(SPIRVContext &context) {
  Type t = Type(spv::Op::OpTypeInt, {8, 1});
  return getUniqueType(context, t);
}
const Type *Type::getTypeUnsignedInt8(SPIRVContext &context) {
  Type t = Type(spv::Op::OpTypeInt, {8, 0});
  return getUniqueType(context, t);
}
const Type *Type::getTypeInt16(SPIRVContext &context) {
  Type t = Type(spv::Op::OpTypeInt, {16, 1});
  return getUniqueType(context, t);
}
const Type *Type::getTypeUnsignedInt16(SPIRVContext &context) {
  Type t = Type(spv::Op::OpTypeInt, {16, 0});
  return getUniqueType(context, t);
}
const Type *Type::getTypeInt32(SPIRVContext &context) {
  Type t = Type(spv::Op::OpTypeInt, {32, 1});
  return getUniqueType(context, t);
}
const Type *Type::getTypeUnsignedInt32(SPIRVContext &context) {
  Type t = Type(spv::Op::OpTypeInt, {32, 0});
  return getUniqueType(context, t);
}
const Type *Type::getTypeInt64(SPIRVContext &context) {
  Type t = Type(spv::Op::OpTypeInt, {64, 1});
  return getUniqueType(context, t);
}
const Type *Type::getTypeUnsignedInt64(SPIRVContext &context) {
  Type t = Type(spv::Op::OpTypeInt, {64, 0});
  return getUniqueType(context, t);
}
const Type *Type::getTypeFloat16(SPIRVContext &context) {
  Type t = Type(spv::Op::OpTypeFloat, {16});
  return getUniqueType(context, t);
}
const Type *Type::getTypeFloat32(SPIRVContext &context) {
  Type t = Type(spv::Op::OpTypeFloat, {32});
  return getUniqueType(context, t);
}
const Type *Type::getTypeFloat64(SPIRVContext &context) {
  Type t = Type(spv::Op::OpTypeFloat, {64});
  return getUniqueType(context, t);
}
const Type *Type::getTypeVector(SPIRVContext &context, uint32_t component_type,
                                uint32_t vec_size) {
  Type t = Type(spv::Op::OpTypeVector, {component_type, vec_size});
  return getUniqueType(context, t);
}
const Type *Type::getTypeVec2(SPIRVContext &context, uint32_t component_type) {
  return getTypeVector(context, component_type, 2u);
}
const Type *Type::getTypeVec3(SPIRVContext &context, uint32_t component_type) {
  return getTypeVector(context, component_type, 3u);
}
const Type *Type::getTypeVec4(SPIRVContext &context, uint32_t component_type) {
  return getTypeVector(context, component_type, 4u);
}
const Type *Type::getTypeMatrix(SPIRVContext &context, uint32_t column_type_id,
                                uint32_t column_count) {
  Type t = Type(spv::Op::OpTypeMatrix, {column_type_id, column_count});
  return getUniqueType(context, t);
}
const Type *Type::getTypeImage(SPIRVContext &context, uint32_t sampled_type,
                               spv::Dim dim, uint32_t depth, uint32_t arrayed,
                               uint32_t ms, uint32_t sampled,
                               spv::ImageFormat image_format,
                               Option<spv::AccessQualifier> access_qualifier) {
  std::vector<uint32_t> args = {
      sampled_type, uint32_t(dim),         depth, arrayed, ms,
      sampled,      uint32_t(image_format)};
  if (access_qualifier.isSome()) {
    args.push_back(static_cast<uint32_t>(access_qualifier.unwrap()));
  }
  Type t = Type(spv::Op::OpTypeImage, args);
  return getUniqueType(context, t);
}
const Type *Type::getTypeSampler(SPIRVContext &context) {
  Type t = Type(spv::Op::OpTypeSampler);
  return getUniqueType(context, t);
}
const Type *Type::getTypeSampledImage(SPIRVContext &context,
                                      uint32_t image_type_id) {
  Type t = Type(spv::Op::OpTypeSampledImage, {image_type_id});
  return getUniqueType(context, t);
}
const Type *Type::getTypeArray(SPIRVContext &context,
                               uint32_t component_type_id, uint32_t len_id) {
  Type t = Type(spv::Op::OpTypeArray, {component_type_id, len_id});
  return getUniqueType(context, t);
}
const Type *Type::getTypeRuntimeArray(SPIRVContext &context,
                                      uint32_t component_type_id) {
  Type t = Type(spv::Op::OpTypeRuntimeArray, {component_type_id});
  return getUniqueType(context, t);
}
const Type *Type::getTypeStruct(SPIRVContext &context,
                                std::initializer_list<uint32_t> members) {
  Type t = Type(spv::Op::OpTypeStruct, std::vector<uint32_t>(members));
  return getUniqueType(context, t);
}
const Type *Type::getTypeOpaque(SPIRVContext &context, std::string name) {
  Type t = Type(spv::Op::OpTypeOpaque, utils::reinterpretStringAsUintVec(name));
  return getUniqueType(context, t);
}
const Type *Type::getTyePointer(SPIRVContext &context,
                                spv::StorageClass storage_class,
                                uint32_t type) {
  Type t = Type(spv::Op::OpTypePointer,
                {static_cast<uint32_t>(storage_class), type});
  return getUniqueType(context, t);
}
const Type *Type::getTypeFunction(SPIRVContext &context, uint32_t return_type,
                                  std::initializer_list<uint32_t> params) {
  std::vector<uint32_t> args = {return_type};
  args.insert(args.end(), params.begin(), params.end());
  Type t = Type(spv::Op::OpTypeFunction, args);
  return getUniqueType(context, t);
}
const Type *Type::getTypeEvent(SPIRVContext &context) {
  Type t = Type(spv::Op::OpTypeEvent);
  return getUniqueType(context, t);
}
const Type *Type::getTypeDeviceEvent(SPIRVContext &context) {
  Type t = Type(spv::Op::OpTypeDeviceEvent);
  return getUniqueType(context, t);
}
const Type *Type::getTypeQueue(SPIRVContext &context) {
  Type t = Type(spv::Op::OpTypeQueue);
  return getUniqueType(context, t);
}
const Type *Type::getTypePipe(SPIRVContext &context,
                              spv::AccessQualifier qualifier) {
  Type t = Type(spv::Op::OpTypePipe, {static_cast<uint32_t>(qualifier)});
  return getUniqueType(context, t);
}
const Type *Type::getTypeForwardPointer(SPIRVContext &context,
                                        uint32_t pointer_type,
                                        spv::StorageClass storage_class) {
  Type t = Type(spv::Op::OpTypeForwardPointer,
                {pointer_type, static_cast<uint32_t>(storage_class)});
  return getUniqueType(context, t);
}
const Type *Type::getTypePipeStorage(SPIRVContext &context) {
  Type t = Type(spv::Op::OpTypePipeStorage);
  return getUniqueType(context, t);
}
const Type *Type::getTypeNamedBarrier(SPIRVContext &context) {
  Type t = Type(spv::Op::OpTypeNamedBarrier);
  return getUniqueType(context, t);
}
const Type *Type::getType(SPIRVContext &context, spv::Op op,
                          std::vector<uint32_t> arg,
                          std::set<const Decoration *> dec) {
  Type t = Type(op, arg, dec);
  return getUniqueType(context, t);
}
bool Type::isBooleanType(const Type *t) {
  return t->getOpcode() == spv::Op::OpTypeBool;
}
bool Type::isIntegerType(const Type *t) {
  return t->getOpcode() == spv::Op::OpTypeInt;
}
bool Type::isFloatType(const Type *t) {
  return t->getOpcode() == spv::Op::OpTypeFloat;
}
bool Type::isNumericalType(const Type *t) {
  return isFloatType(t) || isIntegerType(t);
}
bool Type::isScalarType(const Type *t) {
  return isBooleanType(t) || isNumericalType(t);
}
bool Type::isVectorType(const Type *t) {
  return t->getOpcode() == spv::Op::OpTypeVector;
}
bool Type::isMatrixType(const Type *t) {
  return t->getOpcode() == spv::Op::OpTypeMatrix;
}
bool Type::isArrayType(const Type *t) {
  return t->getOpcode() == spv::Op::OpTypeArray;
}
bool Type::isStructureType(const Type *t) {
  return t->getOpcode() == spv::Op::OpTypeStruct;
}
bool Type::isAggregateType(const Type *t) {
  return isStructureType(t) || isArrayType(t);
}
bool Type::isCompositeType(const Type *t) {
  return isAggregateType(t) || isMatrixType(t) || isVectorType(t);
}
bool Type::isImageType(const Type *t) {
  return t->getOpcode() == spv::Op::OpTypeImage;
}

} // end namespace spirv
} // end namespace clang
