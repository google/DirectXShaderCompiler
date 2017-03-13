//===-- SPIRVContext.h - Context holding SPIR-V codegen data ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_CLANG_SPIRV_SPIRVCONTEXT_H
#define LLVM_CLANG_SPIRV_SPIRVCONTEXT_H

#include <unordered_map>

#include "clang/Frontend/FrontendAction.h"
#include "clang/SPIRV/Decoration.h"
#include "clang/SPIRV/Type.h"

namespace clang {
namespace spirv {

struct TypeHash {
  std::size_t operator()(const Type &t) const {
    // TODO: We could improve this hash function if necessary.
    return std::hash<uint32_t>{}(static_cast<uint32_t>(t.getOpcode()));
  }
};
struct DecorationHash {
  std::size_t operator()(const Decoration &d) const {
    // TODO: We could probably improve this hash function if needed.
    return std::hash<uint32_t>{}(static_cast<uint32_t>(d.getDecorationType()));
  }
};

/// \brief A class for holding various data needed in SPIR-V codegen.
/// It should outlive all SPIR-V codegen components that requires/allocates
/// data.
class SPIRVContext {
public:
  inline SPIRVContext();

  // Disable copy/move (assignment) constructors.
  SPIRVContext(const SPIRVContext &) = delete;
  SPIRVContext(SPIRVContext &&) = delete;
  SPIRVContext &operator=(const SPIRVContext &) = delete;
  SPIRVContext &operator=(SPIRVContext &&) = delete;

  inline uint32_t GetNextId() const;
  inline uint32_t TakeNextId();

  /// \brief Returns the ResultID that defines the given Type. If the type
  /// has not been defined, it will define and store its instruction.
  /// The 'next_id_func' will be called to get the ResultID for the type.
  uint32_t GetResultIdForType(const Type *t,
                              std::function<uint32_t()> next_id_func);

  /// \brief Returns the instruction that defined the given Type.
  /// If no ResultID has been associated with this Type (not defined yet),
  /// the instruction will be created, registered, and returned.
  /// The 'next_id_func' will be called to get the result_id for the type.
  const std::vector<uint32_t> &
  GetInstrForType(const Type *t, std::function<uint32_t()> next_id_func);

  // All the unique types defined in the current context.
  std::unordered_set<Type, TypeHash> ExistingTypes;

  // All the unique Decorations defined in the current context.
  std::unordered_set<Decoration, DecorationHash> ExistingDecorations;

private:
  uint32_t NextId;

  // Maps a ResultID to its SPIR-V instruction.
  std::unordered_map<uint32_t, std::vector<uint32_t>> IdToInstructionMap;

  // Maps a given type to the ResultID that is defined for
  // that type. If a Type* does not exist in the map, the type
  // is not yet defined and is not associated with a ResultID.
  std::unordered_map<const Type *, uint32_t> TypeResultIdMap;
};

SPIRVContext::SPIRVContext() : NextId(1) {}
uint32_t SPIRVContext::GetNextId() const { return NextId; }
uint32_t SPIRVContext::TakeNextId() { return NextId++; }

} // end namespace spirv
} // end namespace clang

#endif