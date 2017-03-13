//===-- Decoration.h - SPIR-V Decoration --*- C++-*------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_CLANG_SPIRV_DECORATION_H
#define LLVM_CLANG_SPIRV_DECORATION_H

#include <unordered_set>
#include <vector>

#include "clang/SPIRV/Option.h"
#include "clang/SPIRV/spirv.hpp"

namespace clang {
namespace spirv {

class SPIRVContext;
class Decoration {
  enum { kInvalidMember = -1 };

public:
  spv::Decoration getDecorationType() const { return id; }
  const std::vector<uint32_t> &getArgs() const { return args; }
  const uint32_t getMemberIndex() const { return memberIndex; }

  static const Decoration *getDecorationRelaxedPrecision(SPIRVContext &ctx);
  static const Decoration *getDecorationSpecId(SPIRVContext &ctx, uint32_t id);
  static const Decoration *getDecorationBlock(SPIRVContext &ctx);
  static const Decoration *getDecorationBufferBlock(SPIRVContext &ctx);
  static const Decoration *getDecorationRowMajor(SPIRVContext &ctx,
                                                 int32_t member_idx);
  static const Decoration *getDecorationColMajor(SPIRVContext &ctx,
                                                 int32_t member_idx);
  static const Decoration *getDecorationArrayStride(SPIRVContext &ctx,
                                                    uint32_t stride);
  static const Decoration *getDecorationMatrixStride(SPIRVContext &ctx,
                                                     uint32_t stride,
                                                     int32_t member_idx);
  static const Decoration *getDecorationGLSLShared(SPIRVContext &ctx);
  static const Decoration *getDecorationGLSLPacked(SPIRVContext &ctx);
  static const Decoration *getDecorationCPacked(SPIRVContext &ctx);
  static const Decoration *
  getDecorationBuiltIn(SPIRVContext &ctx, spv::BuiltIn builtin,
                       int32_t member_idx = kInvalidMember);
  static const Decoration *
  getDecorationNoPerspective(SPIRVContext &ctx,
                             int32_t member_idx = kInvalidMember);
  static const Decoration *
  getDecorationFlat(SPIRVContext &ctx, int32_t member_idx = kInvalidMember);
  static const Decoration *
  getDecorationPatch(SPIRVContext &ctx, int32_t member_idx = kInvalidMember);
  static const Decoration *
  getDecorationCentroid(SPIRVContext &ctx, int32_t member_idx = kInvalidMember);
  static const Decoration *
  getDecorationSample(SPIRVContext &ctx, int32_t member_idx = kInvalidMember);
  static const Decoration *getDecorationInvariant(SPIRVContext &ctx);
  static const Decoration *getDecorationRestrict(SPIRVContext &ctx);
  static const Decoration *getDecorationAliased(SPIRVContext &ctx);
  static const Decoration *
  getDecorationVolatile(SPIRVContext &ctx, int32_t member_idx = kInvalidMember);
  static const Decoration *getDecorationConstant(SPIRVContext &ctx);
  static const Decoration *
  getDecorationCoherent(SPIRVContext &ctx, int32_t member_idx = kInvalidMember);
  static const Decoration *
  getDecorationNonWritable(SPIRVContext &ctx,
                           int32_t member_idx = kInvalidMember);
  static const Decoration *
  getDecorationNonReadable(SPIRVContext &ctx,
                           int32_t member_idx = kInvalidMember);
  static const Decoration *
  getDecorationUniform(SPIRVContext &ctx, int32_t member_idx = kInvalidMember);
  static const Decoration *getDecorationSaturatedConversion(SPIRVContext &ctx);
  static const Decoration *
  getDecorationStream(SPIRVContext &ctx, uint32_t stream_number,
                      int32_t member_idx = kInvalidMember);
  static const Decoration *
  getDecorationLocation(SPIRVContext &ctx, uint32_t location,
                        int32_t member_idx = kInvalidMember);
  static const Decoration *
  getDecorationComponent(SPIRVContext &ctx, uint32_t component,
                         int32_t member_idx = kInvalidMember);
  static const Decoration *getDecorationIndex(SPIRVContext &ctx,
                                              uint32_t index);
  static const Decoration *getDecorationBinding(SPIRVContext &ctx,
                                                uint32_t binding_point);
  static const Decoration *getDecorationDescriptorSet(SPIRVContext &ctx,
                                                      uint32_t set);
  static const Decoration *
  getDecorationOffset(SPIRVContext &ctx, uint32_t byte_offset,
                      int32_t member_idx = kInvalidMember);
  static const Decoration *
  getDecorationXfbBuffer(SPIRVContext &ctx, uint32_t xfb_buf,
                         int32_t member_idx = kInvalidMember);
  static const Decoration *getDecorationXfbStride(SPIRVContext &ctx,
                                                  uint32_t xfb_stride);
  static const Decoration *
  getDecorationFuncParamAttr(SPIRVContext &ctx,
                             spv::FunctionParameterAttribute attr);
  static const Decoration *
  getDecorationFPRoundingMode(SPIRVContext &ctx, spv::FPRoundingMode mode);
  static const Decoration *
  getDecorationFPFastMathMode(SPIRVContext &ctx, spv::FPFastMathModeShift mode);
  static const Decoration *
  getDecorationLinkageAttributes(SPIRVContext &ctx, std::string name,
                                 spv::LinkageType linkage_type);
  static const Decoration *getDecorationNoContraction(SPIRVContext &ctx);
  static const Decoration *getDecorationInputAttachmentIndex(SPIRVContext &ctx,
                                                             uint32_t index);
  static const Decoration *getDecorationAlignment(SPIRVContext &ctx,
                                                  uint32_t alignment);
  static const Decoration *getDecorationMaxByteOffset(SPIRVContext &ctx,
                                                      uint32_t max_byte_offset);
  static const Decoration *getDecorationOverrideCoverageNV(SPIRVContext &ctx);
  static const Decoration *getDecorationPassthroughNV(SPIRVContext &ctx);
  static const Decoration *getDecorationViewportRelativeNV(SPIRVContext &ctx);
  static const Decoration *
  getDecorationSecondaryViewportRelativeNV(SPIRVContext &ctx, uint32_t offset);

  bool operator==(const Decoration &other) const {
    return id == other.id && args == other.args &&
           memberIndex == other.memberIndex;
  }

private:
  /// \brief prevent public APIs from creating Decoration objects.
  Decoration(spv::Decoration dec_id, std::vector<uint32_t> arg = {},
             int32_t idx = kInvalidMember)
      : id(dec_id), args(arg), memberIndex(idx) {}

  /// \brief Sets the index of the structure member to which the decoration
  /// applies.
  void setMemberIndex(int32_t idx) { memberIndex = idx; }

  /// \brief Uses ExistingDecorations to return a unique Decoration.
  static const Decoration *getUniqueDecoration(SPIRVContext &ctx,
                                               Decoration &d);

private:
  // Private members that define a unique SPIR-V Decoration.
  spv::Decoration id;
  std::vector<uint32_t> args;
  // If this is a decoration that applies to a structure member, the index
  // of the member is stored here. It will be kInvalidMember in all other
  // cases. A 32-bit integer is certainly enough to store the member index
  // (see Universal Limits in SPIRV Spec.)
  int32_t memberIndex;
};

} // end namespace spirv
} // end namespace clang

#endif
