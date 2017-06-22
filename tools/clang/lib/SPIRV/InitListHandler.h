//===------- InitListHandler.h - Initializer List Handler -------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//===----------------------------------------------------------------------===//
//
//  This file defines an initalizer list handler that takes in an InitListExpr
//  and emits the corresponding SPIR-V instructions for it.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_SPIRV_INITLISTHANDLER_H
#define LLVM_CLANG_LIB_SPIRV_INITLISTHANDLER_H

#include <deque>
#include <utility>

#include "clang/AST/Expr.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/SPIRV/ModuleBuilder.h"

#include "SPIRVEmitter.h"
#include "TypeTranslator.h"

namespace clang {
namespace spirv {

/// The class for handling initializer lists.
///
/// Initializer lists in HLSL are very flexible; as long as the initializer
/// list provides the exact number of elements required by the type of the
/// object to be initialized, it will highly likely be accepted. To handle
/// such flexibility, composite extraction, recursive composite construction,
/// and proper casting is necessary for some cases. For example:
///
///   float4x4 mat = {scalar, vec1, vec2, vec3, vec2, float2(1, 2), 3, vec4};
/// The first element vector is composed of scalar, vec1, and vec2; the second
/// element vector is composed of vec3 and the first element of vec2; etc.
///
/// The AST is designed to omit the composite extraction and construction. It
/// also does not have casts to the expected types and from lvalues to rvalues.
///
/// Resulting from the above reasons, the logic for handling initalizer lists
/// are complicated. So we have this dedicated class for it. It is built on
/// top of the SPIRVEmitter class and calls into SPIRVEmitter for normal
/// translating tasks. This gives better code structure.
///
/// The logic for handling initalizer lists is largely the following:
///
/// First we flatten() the given initalizer list recursively and put all non-
/// initializer-list AST Exprs into the initializers queue. This handles curly
/// braces of even wired forms like float2x2 mat = {{1.}, {2., {{3.}}}, 4.};
///
/// Then we construct the final SPIR-V composite from the initializer list
/// by traversing the type of the composte. This is done recursively in the
/// depth first search manner, using the type of the composite as the root.
///
/// When we reach a scalar type, we will try to decode a scalar value from the
/// front of the initializers queue. This may trigger composite extraction
/// since the front of the queue may be a vector/etc. The leftover values after
/// the extraction should be retained for the next decoding. Thus, we need
/// another queue, scalars, to keep track of leftover unused scalar values.
/// To adjust properly, when decoding values for a given type, we first try
/// the scalar queue.
///
/// When we reach a composite type, we will try to construct a composite using
/// the scalar values previously extracted and retained in the scalars queue.
/// To optimize, if we have no leftover scalars and a value of the same type at
/// the front of the initializers queue, we use the value as a whole.
class InitListHandler {
public:
  /// Constructs an InitListHandler which uses the given emitter for normal
  /// translation tasks. It will reuse the ModuleBuilder embedded in the given
  /// emitter.
  explicit InitListHandler(SPIRVEmitter &emitter);

  /// Processes the given InitListExpr and returns the <result-id> for the final
  /// SPIR-V value.
  uint32_t process(const InitListExpr *expr);

private:
  /// \brief Wrapper method to create an error message and report it
  /// in the diagnostic engine associated with this consumer.
  template <unsigned N> DiagnosticBuilder emitError(const char (&message)[N]) {
    const auto diagId =
        diags.getCustomDiagID(clang::DiagnosticsEngine::Error, message);
    return diags.Report(diagId);
  }

  /// Flattens the given InitListExpr and puts all non-InitListExpr AST nodes
  /// into initializers.
  void flatten(const InitListExpr *expr);

  /// Decomposes the given Expr and puts all elements into the end of the
  /// scalars queue.
  void decompose(const Expr *expr);

  /// Emits the necessary SPIR-V instructions to create a SPIR-V value of the
  /// given type. The scalars and initializers queue will be used to fetch the
  /// next value.
  uint32_t createInitForType(QualType type);
  uint32_t createInitForBuiltinType(QualType type);
  uint32_t createInitForVectorType(QualType elemType, uint32_t count);
  uint32_t createInitForMatrixType(QualType elemType, uint32_t rowCount,
                                   uint32_t colCount);

private:
  SPIRVEmitter &theEmitter;
  ModuleBuilder &theBuilder;
  TypeTranslator &typeTranslator;
  DiagnosticsEngine &diags;

  /// A queue keeping track of unused AST nodes for initializers
  std::deque<const Expr *> initializers;
  /// A queue keeping track of previously extracted but unused scalars
  std::deque<std::pair<uint32_t, QualType>> scalars;
};

} // end namespace spirv
} // end namespace clang

#endif
