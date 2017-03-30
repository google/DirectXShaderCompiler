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
SPIRVContext::getResultIdForType(const Type *t) {
  assert(t != nullptr);
  uint32_t result_id = 0;

  auto iter = typeResultIdMap.find(t);
  if (iter == typeResultIdMap.end()) {
    // The Type has not been defined yet. Reserve an ID for it.
    result_id = takeNextId();

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
    typeResultIdMap[t] = result_id;
    idToInstructionMap[result_id] = instr;
  } else {
    result_id = iter->second;
  }

  assert(result_id != 0);
  return result_id;
}

const std::vector<uint32_t> &
SPIRVContext::getInstrForType(const Type *t) {
  uint32_t result_id = getResultIdForType(t);
  return idToInstructionMap[result_id];
}

const Type* SPIRVContext::registerType(Type& t) {
  // Insert function will only insert if it doesn't already exist in the set.
  std::unordered_set<Type, TypeHash>::iterator it;
  std::tie(it, std::ignore) = existingTypes.insert(t);
  return &*it;
}

const Decoration* SPIRVContext::registerDecoration(Decoration& d) {
  // Insert function will only insert if it doesn't already exist in the set.
  std::unordered_set<Decoration, DecorationHash>::iterator it;
  std::tie(it, std::ignore) = existingDecorations.insert(d);
  return &*it;
}

} // end namespace spirv
} // end namespace clang
