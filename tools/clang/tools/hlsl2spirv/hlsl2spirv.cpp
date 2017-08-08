//===- hlsl2spirv.cpp ---- Entry point for hlsl2spirv program -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <string>
#include <vector>

#include "dxc/Support/Global.h"
#include "dxc/Support/Unicode.h"
#include "dxc/Support/WinIncludes.h"
#include "dxc/Support/HLSLOptions.h"
#include "dxc/Support/dxcapi.use.h"
#include "dxc/dxcapi.h"
#include "dxc/dxcapi.internal.h"

#include "llvm/Option/OptTable.h"
#include "llvm/Support/raw_ostream.h"

#include "spirv-tools/libspirv.hpp"

using namespace dxc;
using namespace llvm;
using namespace llvm::opt;
using namespace hlsl::options;

namespace {
void ConvertIDxcBlobToUint32(const CComPtr<IDxcBlob> &blob,
                             std::vector<uint32_t> *binaryWords) {
  if (!blob)
    return;
  size_t num32BitWords = (blob->GetBufferSize() + 3) / 4;
  std::string binaryStr((char *)blob->GetBufferPointer(),
                        blob->GetBufferSize());
  binaryStr.resize(num32BitWords * 4, 0);
  binaryWords->resize(num32BitWords, 0);
  memcpy(binaryWords->data(), binaryStr.data(), binaryStr.size());
}

class Hlsl2SpirvContext {
private:
  DxcDllSupport &dllSupport;
  DxcOpts &options;
  CComPtr<IDxcBlob> binaryBlobPtr;
  CComPtr<IDxcBlob> spirvAssemblyBlobPtr;
  std::string spirvAssembly;

public:
  Hlsl2SpirvContext(DxcDllSupport &dxcSupport, DxcOpts &opts)
      : dllSupport(dxcSupport), options(opts) {}

  void Compile();
  void Disassemble();
  void Report();

private:
  void convertSpirvAssemblyToBlob();
};

void Hlsl2SpirvContext::convertSpirvAssemblyToBlob() {
  CComPtr<IDxcLibrary> library;
  IFT(dllSupport.CreateInstance(CLSID_DxcLibrary, &library));
  IFT(library->CreateBlobWithEncodingOnHeapCopy(
      spirvAssembly.data(), spirvAssembly.size(), CP_UTF8,
      (IDxcBlobEncoding **)&spirvAssemblyBlobPtr));
}

void Hlsl2SpirvContext::Report() {
  bool needDisassembly =
      !options.AssemblyCode.empty() || options.OutputObject.empty();

  if (needDisassembly)
    Disassemble();

  if (!options.AssemblyCode.empty()) {
    convertSpirvAssemblyToBlob();
    WriteBlobToFile(spirvAssemblyBlobPtr, StringRefUtf16(options.AssemblyCode));
  }

  if (!options.OutputObject.empty())
    WriteBlobToFile(binaryBlobPtr, StringRefUtf16(options.OutputObject));
  else
    WriteUtf8ToConsole(spirvAssembly.c_str(), spirvAssembly.size(),
                       STD_OUTPUT_HANDLE);
}

void Hlsl2SpirvContext::Disassemble() {
  // First, convert the compiled blob into a vector of uint32_t
  std::vector<uint32_t> words;
  ConvertIDxcBlobToUint32(binaryBlobPtr, &words);

  spvtools::SpirvTools spirvTools(SPV_ENV_VULKAN_1_0);
  spirvTools.SetMessageConsumer(
      [](spv_message_level_t, const char *, const spv_position_t &,
         const char *message) { fprintf(stdout, "%s\n", message); });
  uint32_t options = SPV_BINARY_TO_TEXT_OPTION_FRIENDLY_NAMES;
  if (!spirvTools.Disassemble(words, &spirvAssembly, options))
    throw("Failed to disassemble the SPIR-V binary.");
}

void Hlsl2SpirvContext::Compile() {
  CComPtr<IDxcLibrary> pLibrary;
  CComPtr<IDxcCompiler> pCompiler;
  CComPtr<IDxcOperationResult> pResult;
  CComPtr<IDxcBlobEncoding> pSource;
  CComPtr<IDxcBlobEncoding> pErrorBuffer;
  CComPtr<IDxcIncludeHandler> pIncludeHandler;
  HRESULT resultStatus;

  auto inputFileName = StringRefUtf16(options.InputFile);
  auto entry = StringRefUtf16(options.EntryPoint);
  auto profile = StringRefUtf16(options.TargetProfile);

  std::vector<LPCWSTR> flags;
  flags.push_back(L"-spirv");

  IFT(dllSupport.CreateInstance(CLSID_DxcLibrary, &pLibrary));
  IFT(pLibrary->CreateBlobFromFile(inputFileName, nullptr, &pSource));
  IFT(pLibrary->CreateIncludeHandler(&pIncludeHandler));
  IFT(dllSupport.CreateInstance(CLSID_DxcCompiler, &pCompiler));
  IFT(pCompiler->Compile(pSource, inputFileName, entry, profile, flags.data(),
                         flags.size(), nullptr, 0, pIncludeHandler, &pResult));
  // Get compilation results.
  IFT(pResult->GetStatus(&resultStatus));

  // Get diagnostics string and print warnings and errors to stderr.
  IFT(pResult->GetErrorBuffer(&pErrorBuffer));
  const std::string diagnostics((char *)pErrorBuffer->GetBufferPointer(),
                                pErrorBuffer->GetBufferSize());
  if (!diagnostics.empty())
    fprintf(stderr, "%s\n", diagnostics.c_str());

  if (SUCCEEDED(resultStatus)) {
    IFT(pResult->GetResult(&binaryBlobPtr));
  } else {
    throw("Compilation of the input HLSL via hlsl2spirv failed.");
  }
}

} // anonymous namespace

int __cdecl wmain(int argc, const wchar_t **argv_) {
  const char *pStage = "Operation";
  try {
    pStage = "Argument processing";

    // Parse command line options.
    const OptTable *optionTable = getHlslOptTable();
    MainArgs argStrings(argc, argv_);
    DxcOpts dxcOpts;
    DxcDllSupport dxcSupport;
    dxc::EnsureEnabled(dxcSupport);

    // TODO: We currently do not have an options table for hlsl2spirv.
    // Therefore we have used the DxcOpts class to handle command line
    // arguments.
    // Ideally this must be fixed by:
    // 1- hlsl2spiv having its own Hlsl2SpirvOpts
    // 2- Supporting only the options we actually need (and erroring out for any
    // other option).

    // Read options and check errors.
    {
      std::string errorString;
      llvm::raw_string_ostream errorStream(errorString);
      int optResult =
          ReadDxcOpts(optionTable, DxcFlags, argStrings, dxcOpts, errorStream);
      errorStream.flush();
      if (!errorString.empty()) {
        fprintf(stderr, "hlsl2spirv failed : %s", errorString.data());
      }
      if (optResult != 0) {
        return optResult;
      }
    }

    // Handle help request, which overrides any other processing.
    if (dxcOpts.ShowHelp) {
      std::string helpString;
      llvm::raw_string_ostream helpStream(helpString);
      optionTable->PrintHelp(helpStream, "hlsl2spirv.exe",
                             "HLSL to SPIR-V Compiler");
      helpStream.flush();
      WriteUtf8ToConsoleSizeT(helpString.data(), helpString.size());
      return 0;
    }

    // Apply defaults.
    if (dxcOpts.EntryPoint.empty() && !dxcOpts.RecompileFromBinary) {
      dxcOpts.EntryPoint = "main";
    }

    Hlsl2SpirvContext context(dxcSupport, dxcOpts);
    pStage = "Compilation";
    context.Compile();
    context.Report();
  } catch (const ::hlsl::Exception &hlslException) {
    try {
      const char *msg = hlslException.what();
      // printBuffer is safe to treat as UTF-8 because we use ASCII errors only.
      Unicode::acp_char printBuffer[128];
      if (msg == nullptr || *msg == '\0') {
        sprintf_s(printBuffer, _countof(printBuffer),
                  "Compilation failed - error code 0x%08x.", hlslException.hr);
        msg = printBuffer;
      }
      printf("%s\n", msg);
    } catch (...) {
      printf("%s failed - unable to retrieve error message.\n", pStage);
    }
    return 1;
  } catch (std::bad_alloc &) {
    printf("%s failed - out of memory.\n", pStage);
    return 1;
  } catch (const char *errMsg) {
    printf("%s\n", errMsg);
    return 1;
  } catch (...) {
    printf("%s failed - unknown error.\n", pStage);
    return 1;
  }

  return 0;
}
