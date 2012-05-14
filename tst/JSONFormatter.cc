#include "gtest/gtest.h"
#include "llvm/Support/raw_ostream.h"
#include "JSONFormatter.h"

using namespace FunctionFanout;

class JSONFormatterTest: public ::testing::Test
{
protected:
   static std::string testStrForOutput;
   static llvm::raw_string_ostream testOutput;
   virtual void SetUp()
   {
      testStrForOutput.clear();
   }
   virtual void TearDown()
   {
   }
};
std::string JSONFormatterTest::testStrForOutput;
llvm::raw_string_ostream JSONFormatterTest::testOutput(JSONFormatterTest::testStrForOutput);

TEST_F(JSONFormatterTest, init)
{
   JSONFormatter fmt(&testOutput);

   ASSERT_STREQ("", testStrForOutput.c_str());
}

TEST_F(JSONFormatterTest, empty)
{
   JSONFormatter fmt(&testOutput);

   fmt.BeginSourceFile();
   fmt.EndSourceFile();

   ASSERT_STREQ("{\n}\n", testStrForOutput.c_str());
}
