//===- unittests/SPIRV/WholeFileCheck.h ---- WholeFileCheck Test Fixture --===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <algorithm>
#include <fstream>

#include "dxc/Support/Global.h"
#include "dxc/Support/WinIncludes.h"
#include "dxc/Support/dxcapi.use.h"
#include "spirv-tools/libspirv.hpp"
#include "gtest/gtest.h"

#include "SpirvTestOptions.h"

namespace {
const std::string hlslStartLabel = "// Run:";
const std::string spirvStartLabel = "// CHECK-WHOLE-SPIR-V";
const std::string hlslSuffix = ".hlsl";
}

/// \brief The purpose of the this test class is to take in an input file with
/// the following format:
///
///    // Comments...
///    // More comments...
///    // Run: %dxc -T ps_6_0 -E main
///    ...
///    <HLSL code goes here>
///    ...
///    // CHECK-WHOLE-SPIR-V
///    ...
///    <SPIR-V code goes here>
///    ...
///
/// The first part of the file is read in as the HLSL source. It is fed to the
/// DXC compiler with the SPIR-V Generation option. The resulting SPIR-V binary
/// is then fed to the SPIR-V disassembler (via SPIR-V Tools) to get a
/// plain-text SPIR-V module. The resulting SPIR-V plain-text module is compared
/// to the second part of the input file. If these match, the test is marked as
/// a PASS, and marked as a FAILED otherwise.
class WholeFileTest : public ::testing::Test {
public:
  WholeFileTest();

  /// \brief Runs a WHOLE-FILE-TEST! (See class description for more info)
  /// Returns true if the test passes; false otherwise.
  /// Since SPIR-V headers may change, a test is more robust if the
  /// disassembler does not include the header.
  /// It is also important that all generated SPIR-V code is valid. Users of
  /// WholeFileTest may choose not to run the SPIR-V Validator (for cases where
  /// a certain feature has not been added to the Validator yet).
  bool runWholeFileTest(std::string path, bool generateHeader = false,
                        bool runSpirvValidation = true);

  /// \brief Reads in the given input file. Separates it into 2 parts.
  /// Writes the HLSL source into a file (to be used by the compiler).
  /// Stores the SPIR-V portion into the <expectedSPIRV> member variable.
  /// Returns true on success, and false on failure.
  bool parseInputFile();

  /// \brief Passes the HLSL input to the DXC compiler with SPIR-V CodeGen.
  /// Writes the SPIR-V Binary to the output file.
  /// Returns true on success, and false on failure.
  bool runCompilerWithSpirvGeneration();

  /// \brief Passes the SPIR-V Binary to the disassembler.
  bool disassembleSpirvBinary(bool generatedHeader = false);

  /// \brief Runs the SPIR-V tools Validator on the SPIR-V Binary.
  /// Returns true if validation is successful; false otherwise.
  bool ValidateSpirvBinary();

  /// \brief Compares the expected and the generated SPIR-V code.
  /// Returns true if they match, and false otherwise.
  bool compareExpectedSpirvAndGeneratedSpirv();

private:
  /// \brief Helper function that writes the given string content to the given
  /// file. Returns true on success, and false otherwise.
  bool writeToFile(std::string path, const std::string &content);

  /// \brief Parses the Target Profile and Entry Point from the Run command
  bool processRunCommandArgs(std::string &runCommandLine);

  /// \brief Converts an IDxcBlob that is the output of "%DXC -spirv" into a
  /// vector of 32-bit unsigned integers that can be passed into the
  /// disassembler. Stores the results in <generatedBinary>.
  void convertIDxcBlobToUint32(CComPtr<IDxcBlob> &blob);

  /// \brief Returns the absolute path to the input file of the test.
  std::string WholeFileTest::getAbsPathOfInputDataFile(std::string filename);

  std::string targetProfile;             ///< Target profile (argument of -T)
  std::string entryPoint;                ///< Entry point name (argument of -E)
  std::string inputFilePath;             ///< Path to the input test file
  std::string hlslSourceFilePath;        ///< Path to the input HLSL file
  std::vector<uint32_t> generatedBinary; ///< The generated SPIR-V Binary
  std::string expectedSpirvString;       ///< Expected SPIR-V parsed from input
  std::string generatedSpirvString;      ///< Disassembled binary (SPIR-V code)
  spvtools::SpirvTools spirvTools;       ///< SPIR-V Tools used by the test
};

WholeFileTest::WholeFileTest() : spirvTools(SPV_ENV_UNIVERSAL_1_0) {
  spirvTools.SetMessageConsumer(
      [](spv_message_level_t, const char *, const spv_position_t &,
         const char *message) { fprintf(stdout, "%s\n", message); });
}

bool WholeFileTest::writeToFile(std::string path, const std::string &content) {
  std::ofstream outputFile;
  outputFile.exceptions(std::ofstream::failbit);
  try {
    outputFile.open(path);
    outputFile << content;
    outputFile.close();
  } catch (...) {
    // An exception was thrown when opening/writing to output file.
    fprintf(stderr, "Error: unable to open/write to file %s\n", path.c_str());
    return false;
  }
  return true;
}

bool WholeFileTest::processRunCommandArgs(std::string &runCommandLine) {
  std::istringstream buf(runCommandLine);
  std::istream_iterator<std::string> start(buf), end;
  std::vector<std::string> tokens(start, end);
  if (tokens[1].find("Run") == std::string::npos ||
      tokens[2].find("%dxc") == std::string::npos) {
    fprintf(stderr, "The only supported format is: \"// Run: %%dxc -T "
                    "<profile> -E <entry>\"\n");
    return false;
  }

  for (size_t i = 0; i < tokens.size(); ++i) {
    if (tokens[i] == "-T" && i + 1 < tokens.size())
      targetProfile = tokens[i + 1];
    else if (tokens[i] == "-E" && i + 1 < tokens.size())
      entryPoint = tokens[i + 1];
  }
  if (targetProfile.empty()) {
    fprintf(stderr, "Error: Missing target profile argument (-T).\n");
    return false;
  }
  if (entryPoint.empty()) {
    fprintf(stderr, "Error: Missing entry point argument (-E).\n");
    return false;
  }
  return true;
}

bool WholeFileTest::parseInputFile() {
  bool parse = false;
  std::ostringstream outString;
  std::ifstream inputFile;
  inputFile.exceptions(std::ifstream::failbit);
  try {
    inputFile.open(inputFilePath);
    for (std::string line; !inputFile.eof() && std::getline(inputFile, line);) {
      if (line.find(hlslStartLabel) != std::string::npos) {
        if (processRunCommandArgs(line)) {
          // Parsing was successful. Start reading HLSL.
          parse = true;
        } else {
          // An error has occured when parsing the Run command.
          return false;
        }
      } else if (line.find(spirvStartLabel) != std::string::npos) {
        // HLSL source has ended. Write it out.
        if (!writeToFile(hlslSourceFilePath, outString.str())) {
          return false;
        }
        // SPIR-V source starts on the next line.
        outString.str("");
        outString.clear();
      } else if (parse) {
        outString << line << std::endl;
      }
    }

    if (!parse) {
      fprintf(stderr, "Error: Missing Run command.\n");
      return false;
    }

    // Reached the end of the file. SPIR-V source has ended. Store it for
    // comparison.
    expectedSpirvString = outString.str();

    // Close the input file.
    inputFile.close();
  } catch (...) {
    fprintf(
        stderr,
        "Error: Exception occurred while opening/reading the input file %s\n",
        inputFilePath.c_str());
    return false;
  }

  // Everything was successful.
  return true;
}

bool WholeFileTest::runCompilerWithSpirvGeneration() {
  std::wstring srcFile(hlslSourceFilePath.begin(), hlslSourceFilePath.end());
  std::wstring entry(entryPoint.begin(), entryPoint.end());
  std::wstring profile(targetProfile.begin(), targetProfile.end());
  bool success = true;

  try {
    dxc::DxcDllSupport dllSupport;
    IFT(dllSupport.Initialize());

    CComPtr<IDxcLibrary> pLibrary;
    CComPtr<IDxcCompiler> pCompiler;
    CComPtr<IDxcOperationResult> pResult;
    CComPtr<IDxcBlobEncoding> pSource;
    CComPtr<IDxcBlobEncoding> pErrorBuffer;
    CComPtr<IDxcBlob> pCompiledBlob;
    CComPtr<IDxcIncludeHandler> pIncludeHandler;
    HRESULT resultStatus;

    std::vector<LPCWSTR> flags;
    flags.push_back(L"-E");
    flags.push_back(entry.c_str());
    flags.push_back(L"-T");
    flags.push_back(profile.c_str());
    flags.push_back(L"-spirv");

    IFT(dllSupport.CreateInstance(CLSID_DxcLibrary, &pLibrary));
    IFT(pLibrary->CreateBlobFromFile(srcFile.c_str(), nullptr, &pSource));
    IFT(pLibrary->CreateIncludeHandler(&pIncludeHandler));
    IFT(dllSupport.CreateInstance(CLSID_DxcCompiler, &pCompiler));
    IFT(pCompiler->Compile(pSource, srcFile.c_str(), entry.c_str(),
                           profile.c_str(), flags.data(), flags.size(), nullptr,
                           0, pIncludeHandler, &pResult));
    IFT(pResult->GetStatus(&resultStatus));

    if (SUCCEEDED(resultStatus)) {
      CComPtr<IDxcBlobEncoding> pStdErr;
      IFT(pResult->GetResult(&pCompiledBlob));
      convertIDxcBlobToUint32(pCompiledBlob);
      success = true;
    } else {
      IFT(pResult->GetErrorBuffer(&pErrorBuffer));
      fprintf(stderr, "%s\n", (char *)pErrorBuffer->GetBufferPointer());
      success = false;
    }
  } catch (...) {
    // An exception has occured while running the compiler with SPIR-V
    // Generation
    success = false;
  }

  return success;
}

bool WholeFileTest::disassembleSpirvBinary(bool generateHeader) {
  uint32_t options = SPV_BINARY_TO_TEXT_OPTION_FRIENDLY_NAMES;
  if (!generateHeader)
    options |= SPV_BINARY_TO_TEXT_OPTION_NO_HEADER;
  return spirvTools.Disassemble(generatedBinary, &generatedSpirvString,
                                options);
}

bool WholeFileTest::ValidateSpirvBinary() {
  return spirvTools.Validate(generatedBinary);
}

void WholeFileTest::convertIDxcBlobToUint32(CComPtr<IDxcBlob> &blob) {
  size_t num32BitWords = (blob->GetBufferSize() / 4);
  if (blob->GetBufferSize() % 4 > 0)
    num32BitWords++;
  std::string binaryStr((char *)blob->GetBufferPointer(),
                        blob->GetBufferSize());
  binaryStr.resize(num32BitWords * 4, 0);
  generatedBinary.resize(num32BitWords, 0);
  for (size_t i = 0; i < num32BitWords; ++i) {
    generatedBinary[i] =
        (reinterpret_cast<const uint32_t *>(binaryStr.data()))[i];
  }
}

bool WholeFileTest::compareExpectedSpirvAndGeneratedSpirv() {
  return generatedSpirvString == expectedSpirvString;
}

std::string WholeFileTest::getAbsPathOfInputDataFile(std::string filename) {
  std::string path = clang::spirv::testOptions::inputDataDir;
  char sep = '/';

#if defined(_WIN32)
  sep = '\\';
  std::replace(path.begin(), path.end(), '/', '\\');
#endif

  if (path[path.size() - 1] != sep) {
    path = path + sep;
  }
  path += filename;
  return path;
}

bool WholeFileTest::runWholeFileTest(std::string filename, bool generateHeader,
                                     bool runSpirvValidation) {
  inputFilePath = getAbsPathOfInputDataFile(filename);
  hlslSourceFilePath = inputFilePath + hlslSuffix;

  bool success = true;

  // Parse the input file.
  success = success && parseInputFile();

  // Feed the HLSL source into the Compiler.
  success = success && runCompilerWithSpirvGeneration();

  // Disassemble the generated SPIR-V binary.
  success = success && disassembleSpirvBinary(generateHeader);

  // Run SPIR-V validation if requested.
  if (runSpirvValidation) {
    success = success && ValidateSpirvBinary();
  }

  // Compare the expected and the generted SPIR-V code.
  success = success && compareExpectedSpirvAndGeneratedSpirv();

  return success;
}
