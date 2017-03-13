//===--- Decoration.cpp - SPIR-V Decoration implementation-----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "clang/SPIRV/Decoration.h"
#include "clang/SPIRV/SPIRVContext.h"
#include "clang/SPIRV/Utils.h"
#include "clang/SPIRV/spirv.hpp"
#include "llvm/llvm_assert/assert.h"

namespace clang {
namespace spirv {

const Decoration *Decoration::getUniqueDecoration(SPIRVContext &context,
                                                  Decoration &d) {
  // Insert function will only insert if it doesn't already exist in the set.
  context.ExistingDecorations.insert(d);
  return &(*context.ExistingDecorations.find(d));
}
const Decoration *
Decoration::getDecorationRelaxedPrecision(SPIRVContext &context) {
  Decoration d = Decoration(spv::Decoration::RelaxedPrecision);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationSpecId(SPIRVContext &context,
                                                  uint32_t id) {
  Decoration d = Decoration(spv::Decoration::SpecId, {id});
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationBlock(SPIRVContext &context) {
  Decoration d = Decoration(spv::Decoration::Block);
  return getUniqueDecoration(context, d);
}

const Decoration *Decoration::getDecorationBufferBlock(SPIRVContext &context) {
  Decoration d = Decoration(spv::Decoration::BufferBlock);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationRowMajor(SPIRVContext &context,
                                                    int32_t member_idx) {
  Decoration d = Decoration(spv::Decoration::RowMajor);
  d.setMemberIndex(member_idx);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationColMajor(SPIRVContext &context,
                                                    int32_t member_idx) {
  Decoration d = Decoration(spv::Decoration::ColMajor);
  d.setMemberIndex(member_idx);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationArrayStride(SPIRVContext &context,
                                                       uint32_t stride) {
  Decoration d = Decoration(spv::Decoration::ArrayStride, {stride});
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationMatrixStride(SPIRVContext &context,
                                                        uint32_t stride,
                                                        int32_t member_idx) {
  Decoration d = Decoration(spv::Decoration::MatrixStride, {stride});
  d.setMemberIndex(member_idx);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationGLSLShared(SPIRVContext &context) {
  Decoration d = Decoration(spv::Decoration::GLSLShared);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationGLSLPacked(SPIRVContext &context) {
  Decoration d = Decoration(spv::Decoration::GLSLPacked);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationCPacked(SPIRVContext &context) {
  Decoration d = Decoration(spv::Decoration::CPacked);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationBuiltIn(SPIRVContext &context,
                                                   spv::BuiltIn builtin,
                                                   int32_t member_idx) {
  Decoration d =
      Decoration(spv::Decoration::BuiltIn, {static_cast<uint32_t>(builtin)});
  d.setMemberIndex(member_idx);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationNoPerspective(SPIRVContext &context,
                                                         int32_t member_idx) {
  Decoration d = Decoration(spv::Decoration::NoPerspective);
  d.setMemberIndex(member_idx);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationFlat(SPIRVContext &context,
                                                int32_t member_idx) {
  Decoration d = Decoration(spv::Decoration::Flat);
  d.setMemberIndex(member_idx);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationPatch(SPIRVContext &context,
                                                 int32_t member_idx) {
  Decoration d = Decoration(spv::Decoration::Patch);
  d.setMemberIndex(member_idx);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationCentroid(SPIRVContext &context,
                                                    int32_t member_idx) {
  Decoration d = Decoration(spv::Decoration::Centroid);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationSample(SPIRVContext &context,
                                                  int32_t member_idx) {
  Decoration d = Decoration(spv::Decoration::Sample);
  d.setMemberIndex(member_idx);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationInvariant(SPIRVContext &context) {
  Decoration d = Decoration(spv::Decoration::Invariant);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationRestrict(SPIRVContext &context) {
  Decoration d = Decoration(spv::Decoration::Restrict);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationAliased(SPIRVContext &context) {
  Decoration d = Decoration(spv::Decoration::Aliased);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationVolatile(SPIRVContext &context,
                                                    int32_t member_idx) {
  Decoration d = Decoration(spv::Decoration::Volatile);
  d.setMemberIndex(member_idx);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationConstant(SPIRVContext &context) {
  Decoration d = Decoration(spv::Decoration::Constant);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationCoherent(SPIRVContext &context,
                                                    int32_t member_idx) {
  Decoration d = Decoration(spv::Decoration::Coherent);
  d.setMemberIndex(member_idx);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationNonWritable(SPIRVContext &context,
                                                       int32_t member_idx) {
  Decoration d = Decoration(spv::Decoration::NonWritable);
  d.setMemberIndex(member_idx);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationNonReadable(SPIRVContext &context,
                                                       int32_t member_idx) {
  Decoration d = Decoration(spv::Decoration::NonReadable);
  d.setMemberIndex(member_idx);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationUniform(SPIRVContext &context,
                                                   int32_t member_idx) {
  Decoration d = Decoration(spv::Decoration::Uniform);
  d.setMemberIndex(member_idx);
  return getUniqueDecoration(context, d);
}
const Decoration *
Decoration::getDecorationSaturatedConversion(SPIRVContext &context) {
  Decoration d = Decoration(spv::Decoration::SaturatedConversion);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationStream(SPIRVContext &context,
                                                  uint32_t stream_number,
                                                  int32_t member_idx) {
  Decoration d = Decoration(spv::Decoration::Stream, {stream_number});
  d.setMemberIndex(member_idx);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationLocation(SPIRVContext &context,
                                                    uint32_t location,
                                                    int32_t member_idx) {
  Decoration d = Decoration(spv::Decoration::Location, {location});
  d.setMemberIndex(member_idx);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationComponent(SPIRVContext &context,
                                                     uint32_t component,
                                                     int32_t member_idx) {
  Decoration d = Decoration(spv::Decoration::Component, {component});
  d.setMemberIndex(member_idx);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationIndex(SPIRVContext &context,
                                                 uint32_t index) {
  Decoration d = Decoration(spv::Decoration::Index, {index});
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationBinding(SPIRVContext &context,
                                                   uint32_t binding_point) {
  Decoration d = Decoration(spv::Decoration::Binding, {binding_point});
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationDescriptorSet(SPIRVContext &context,
                                                         uint32_t set) {
  Decoration d = Decoration(spv::Decoration::DescriptorSet, {set});
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationOffset(SPIRVContext &context,
                                                  uint32_t byte_offset,
                                                  int32_t member_idx) {
  Decoration d = Decoration(spv::Decoration::Offset, {byte_offset});
  d.setMemberIndex(member_idx);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationXfbBuffer(SPIRVContext &context,
                                                     uint32_t xfb_buf,
                                                     int32_t member_idx) {
  Decoration d = Decoration(spv::Decoration::XfbBuffer, {xfb_buf});
  d.setMemberIndex(member_idx);
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationXfbStride(SPIRVContext &context,
                                                     uint32_t xfb_stride) {
  Decoration d = Decoration(spv::Decoration::XfbStride, {xfb_stride});
  return getUniqueDecoration(context, d);
}
const Decoration *
Decoration::getDecorationFuncParamAttr(SPIRVContext &context,
                                       spv::FunctionParameterAttribute attr) {
  Decoration d =
      Decoration(spv::Decoration::FuncParamAttr, {static_cast<uint32_t>(attr)});
  return getUniqueDecoration(context, d);
}
const Decoration *
Decoration::getDecorationFPRoundingMode(SPIRVContext &context,
                                        spv::FPRoundingMode mode) {
  Decoration d = Decoration(spv::Decoration::FPRoundingMode,
                            {static_cast<uint32_t>(mode)});
  return getUniqueDecoration(context, d);
}
const Decoration *
Decoration::getDecorationFPFastMathMode(SPIRVContext &context,
                                        spv::FPFastMathModeShift mode) {
  Decoration d = Decoration(spv::Decoration::FPFastMathMode,
                            {static_cast<uint32_t>(mode)});
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationLinkageAttributes(
    SPIRVContext &context, std::string name, spv::LinkageType linkage_type) {
  std::vector<uint32_t> args = utils::reinterpretStringAsUintVec(name);
  args.push_back(static_cast<uint32_t>(linkage_type));
  Decoration d = Decoration(spv::Decoration::LinkageAttributes, args);
  return getUniqueDecoration(context, d);
}
const Decoration *
Decoration::getDecorationNoContraction(SPIRVContext &context) {
  Decoration d = Decoration(spv::Decoration::NoContraction);
  return getUniqueDecoration(context, d);
}
const Decoration *
Decoration::getDecorationInputAttachmentIndex(SPIRVContext &context,
                                              uint32_t index) {
  Decoration d = Decoration(spv::Decoration::InputAttachmentIndex, {index});
  return getUniqueDecoration(context, d);
}
const Decoration *Decoration::getDecorationAlignment(SPIRVContext &context,
                                                     uint32_t alignment) {
  Decoration d = Decoration(spv::Decoration::Alignment, {alignment});
  return getUniqueDecoration(context, d);
}
const Decoration *
Decoration::getDecorationMaxByteOffset(SPIRVContext &context,
                                       uint32_t max_byte_offset) {
  Decoration d = Decoration(spv::Decoration::MaxByteOffset, {max_byte_offset});
  return getUniqueDecoration(context, d);
}
const Decoration *
Decoration::getDecorationOverrideCoverageNV(SPIRVContext &context) {
  Decoration d = Decoration(spv::Decoration::OverrideCoverageNV);
  return getUniqueDecoration(context, d);
}
const Decoration *
Decoration::getDecorationPassthroughNV(SPIRVContext &context) {
  Decoration d = Decoration(spv::Decoration::PassthroughNV);
  return getUniqueDecoration(context, d);
}
const Decoration *
Decoration::getDecorationViewportRelativeNV(SPIRVContext &context) {
  Decoration d = Decoration(spv::Decoration::ViewportRelativeNV);
  return getUniqueDecoration(context, d);
}
const Decoration *
Decoration::getDecorationSecondaryViewportRelativeNV(SPIRVContext &context,
                                                     uint32_t offset) {
  Decoration d = Decoration(spv::Decoration::SecondaryViewportRelativeNV);
  return getUniqueDecoration(context, d);
}

} // end namespace spirv
} // end namespace clang
