#ifndef JSONFORMATTER_H_
#define JSONFORMATTER_H_
#include <string>
#include <vector>

namespace FunctionFanout {

class JSONFormatter
{
public:
   JSONFormatter(llvm::raw_ostream*);
   virtual ~JSONFormatter();
   virtual void BeginSourceFile();
   virtual void EndSourceFile();
   typedef std::vector<std::string> params_t;
   virtual void AddDefinition(const std::string name, const std::string type, const params_t& params);
   virtual void EndDefinition();
   virtual void AddCallee(const std::string name, const std::string type, const params_t& params);
private:
   llvm::raw_ostream& ost_;
   unsigned num_of_definitions_;
   unsigned num_of_callees_;

   void FormatFunction(const std::string name, const std::string type, const params_t& params);
};

} /* namespace FunctionFanout */
#endif /* JSONFORMATTER_H_ */
