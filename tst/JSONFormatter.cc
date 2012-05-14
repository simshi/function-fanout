#include "gtest/gtest.h"
#include "llvm/Support/raw_ostream.h"
#include "JSONFormatter.h"

using namespace FunctionFanout;

TEST(JSONFormatter, empty)
{
   std::string testStrForOutput;
   llvm::raw_string_ostream testOutput(testStrForOutput);
   JSONFormatter fmt(testOutput);
   //fmt.EndSourceFileAction();

   ASSERT_STREQ("{}", testStrForOutput.c_str());
}
