#include "gtest/gtest.h"
#include "llvm/Support/raw_ostream.h"
#include "JSONFormatter.h"

using namespace FunctionFanout;

class JSONFormatterTest: public ::testing::Test
{
protected:
   static std::string testStrForOutput;
   static llvm::raw_string_ostream testOutput;
   static std::vector<std::string> params0, params1, params3;

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
static std::string params_list[] = { "param_type1", "param_type2", "param_type3" };
std::vector<std::string> JSONFormatterTest::params0;
std::vector<std::string> JSONFormatterTest::params1(params_list, params_list + 1);
std::vector<std::string> JSONFormatterTest::params3(params_list, params_list + 3);

TEST_F(JSONFormatterTest, init)
{
   JSONFormatter fmt(&testOutput);

   ASSERT_STREQ("", testOutput.str().c_str());
}

#define STR_FILE_BEGIN "{\n"
#define STR_FILE_END "}\n"
TEST_F(JSONFormatterTest, empty)
{
   JSONFormatter fmt(&testOutput);

   fmt.BeginSourceFile();
   fmt.EndSourceFile();

   ASSERT_STREQ(STR_FILE_BEGIN STR_FILE_END, testOutput.str().c_str());
}

#define STR_FUNC0_OPEN "\"type0 func0()\":["
TEST_F(JSONFormatterTest, one_definition)
{
   JSONFormatter fmt(&testOutput);

   fmt.AddDefinition("func0", "type0", params0);

   ASSERT_STREQ(STR_FUNC0_OPEN, testOutput.str().c_str());
}

#define STR_FUNC_END "]"
#define STR_FUNC1_OPEN "\"type1 func1(param_type1)\":["
#define STR_FUNC1 STR_FUNC1_OPEN STR_FUNC_END
TEST_F(JSONFormatterTest, one_definition_with_one_param)
{
   JSONFormatter fmt(&testOutput);

   fmt.AddDefinition("func1", "type1", params1);
   fmt.EndDefinition();

   ASSERT_STREQ(STR_FUNC1, testOutput.str().c_str());
}

#define STR_FUNC3_OPEN "\"type3 func3(param_type1, param_type2, param_type3)\":["
#define STR_FUNC3 STR_FUNC3_OPEN STR_FUNC_END
TEST_F(JSONFormatterTest, one_definition_with_3_params)
{
   JSONFormatter fmt(&testOutput);

   fmt.AddDefinition("func3", "type3", params3);
   fmt.EndDefinition();

   ASSERT_STREQ(STR_FUNC3, testOutput.str().c_str());
}

#define STR_FUNC0 STR_FUNC0_OPEN STR_FUNC_END
#define STR_FUNC_SEP ",\n"
TEST_F(JSONFormatterTest, 2_definitions)
{
   JSONFormatter fmt(&testOutput);

   fmt.AddDefinition("func0", "type0", params0);
   fmt.EndDefinition();
   fmt.AddDefinition("func0", "type0", params0);

   ASSERT_STREQ(STR_FUNC0 STR_FUNC_SEP STR_FUNC0_OPEN, testOutput.str().c_str());
}

TEST_F(JSONFormatterTest, 3_definitions_with_different_params)
{
   JSONFormatter fmt(&testOutput);

   fmt.BeginSourceFile();
   fmt.AddDefinition("func1", "type1", params1);
   fmt.EndDefinition();
   fmt.AddDefinition("func0", "type0", params0);
   fmt.EndDefinition();
   fmt.AddDefinition("func3", "type3", params3);
   fmt.EndDefinition();
   fmt.EndSourceFile();

   ASSERT_STREQ(STR_FILE_BEGIN STR_FUNC1 STR_FUNC_SEP STR_FUNC0 STR_FUNC_SEP STR_FUNC3 STR_FILE_END,
                testOutput.str().c_str());
}
