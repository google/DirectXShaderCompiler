//===- FileTestFixture.cpp ------------- File Test Fixture Implementation -===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <fstream>

#include "FileTestFixture.h"
#include "FileTestUtils.h"

#include "effcee/effcee.h"

namespace clang {
namespace spirv {

namespace {
const char hlslStartLabel[] = "// Run:";
const std::vector<std::string> checkCommands = {"CHECK:",       "CHECK-NEXT:",
                                                "CHECK-SAME:",  "CHECK-DAG:",
                                                "CHECK-LABEL:", "CHECK-NOT:"};

/// \brief Checks if the given string is any of the valid effcee CHECK commands.
bool isCheckCommand(std::string &s) {
  return std::any_of(checkCommands.begin(), checkCommands.end(),
                     [&s](const std::string command) {
                       return s.compare(0, command.length(), command) == 0;
                     });
}

} // end anonymous namespace

bool FileTest::parseInputFile() {
  bool foundRunCommand = false;
  bool foundCheckCommands = false;
  std::ostringstream checkCommandStream;
  std::ifstream inputFile;
  inputFile.exceptions(std::ifstream::failbit);
  try {
    inputFile.open(inputFilePath);
    for (std::string line; !inputFile.eof() && std::getline(inputFile, line);) {
      if (line.find(hlslStartLabel) != std::string::npos) {
        foundRunCommand = true;
        if (!fileTestUtils::processRunCommandArgs(line, &targetProfile,
                                                  &entryPoint)) {
          // An error has occured when parsing the Run command.
          return false;
        }
      }
      // Strip the leading "//" if any
      if (line.size() > 2u && line[0] == '/' && line[1] == '/') {
        line = line.substr(2);
      }
      // Skip any leading whitespace
      size_t found = line.find_first_not_of(" \t");
      if (found != std::string::npos) {
        line = line.substr(found);
      }

      if (isCheckCommand(line)) {
        foundCheckCommands = true;
        checkCommandStream << line << std::endl;
      }
    }

    if (!foundRunCommand) {
      fprintf(stderr, "Error: Missing \"Run:\" command.\n");
      return false;
    }
    if (!foundCheckCommands) {
      fprintf(stderr, "Error: No CHECK commands were found.\n");
      return false;
    }

    // Reached the end of the file. Check commands have ended.
    // Store them so they can be passed to effcee.
    checkCommands = checkCommandStream.str();

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

void FileTest::runFileTest(llvm::StringRef filename, bool runSpirvValidation) {
  inputFilePath = fileTestUtils::getAbsPathOfInputDataFile(filename);

  // Parse the input file.
  ASSERT_TRUE(parseInputFile());

  // Feed the HLSL source into the Compiler.
  ASSERT_TRUE(fileTestUtils::runCompilerWithSpirvGeneration(
      inputFilePath, entryPoint, targetProfile, &generatedBinary));

  // Disassemble the generated SPIR-V binary.
  ASSERT_TRUE(fileTestUtils::disassembleSpirvBinary(
      generatedBinary, &generatedSpirvAsm, true /* generateHeader */));

  // Run CHECK commands via effcee.
  auto result = effcee::Match(generatedSpirvAsm, checkCommands,
                              effcee::Options().SetChecksName("checks"));

  // Print effcee's error message (if any).
  if (result.status() != effcee::Result::Status::Ok) {
    fprintf(stderr, "%s\n", result.message().c_str());
  }

  // All checks must have passed.
  ASSERT_EQ(result.status(), effcee::Result::Status::Ok);

  // Run SPIR-V validation if requested.
  if (runSpirvValidation) {
    EXPECT_TRUE(fileTestUtils::validateSpirvBinary(generatedBinary));
  }
}

} // end namespace spirv
} // end namespace clang
