#include "llvm/Support/raw_ostream.h"
#include "JSONFormatter.h"

namespace FunctionFanout {

JSONFormatter::JSONFormatter(llvm::raw_ostream* ost) :
            ost_(*ost), num_of_definitions_(0)
{

}

JSONFormatter::~JSONFormatter()
{
}

void JSONFormatter::BeginSourceFile()
{
   ost_ << "{\n";
}

void JSONFormatter::EndSourceFile()
{
   ost_ << "}\n";
   ost_.flush();
}

void JSONFormatter::AddDefinition(const std::string name, const std::string type, const params_t& params)
{
   if (num_of_definitions_ > 0) ost_ << ",\n";

   ost_ << "\"" << type << " " << name << "(";

   std::vector<std::string>::const_iterator it = params.begin();
   if (it != params.end()) {
      ost_ << *it;

      for (++it; it != params.end(); ++it) {
         ost_ << ", " << *it;
      }
   }

   ost_ << ")\":[";
   ++num_of_definitions_;
}

void JSONFormatter::EndDefinition()
{
   ost_ << "]";
}

} /* namespace FunctionFanout */
