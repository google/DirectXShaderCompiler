//===--- SPIRVContext.cpp - SPIR-V SPIRVContext implementation-------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "clang/SPIRV/SPIRVContext.h"
#include "llvm/llvm_assert/assert.h"

namespace clang {
namespace spirv {

uint32_t
SPIRVContext::GetResultIdForType(const Type *t,
                                 std::function<uint32_t()> get_id_func) {
  assert(t != nullptr);
  uint32_t result_id = 0;

  auto iter = TypeResultIdMap.find(t);
  if (iter == TypeResultIdMap.end()) {
    // The Type has not been defined yet. Reserve an ID for it.
    result_id = get_id_func();

    // Make the instruction from the Type words
    std::vector<uint32_t> instr;
    instr.push_back(static_cast<uint32_t>(t->getOpcode()));
    // OpTypeForwardPointer is the only OpTypeXXX that does not have a
    // result_id.
    if (t->getOpcode() != spv::Op::OpTypeForwardPointer) {
      instr.push_back(result_id);
    }
    for (const auto &word : t->getArgs()) {
      instr.push_back(word);
    }
    instr[0] |= static_cast<uint16_t>(t->getArgs().size()) << 16;

    // Register it.
    TypeResultIdMap[t] = result_id;
    IdToInstructionMap[result_id] = instr;
  } else {
    result_id = iter->second;
  }

  assert(result_id != 0);
  return result_id;
}

const std::vector<uint32_t> &
SPIRVContext::GetInstrForType(const Type *t,
                              std::function<uint32_t()> get_id_func) {
  uint32_t result_id = GetResultIdForType(t, get_id_func);
  return IdToInstructionMap[result_id];
}

} // end namespace spirv
} // end namespace clang
