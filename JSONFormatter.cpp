#include "llvm/Support/raw_ostream.h"
#include "JSONFormatter.h"

namespace FunctionFanout {

JSONFormatter::JSONFormatter(llvm::raw_ostream& ost) :
            ost_(ost)
{
   // TODO Auto-generated constructor stub

}

JSONFormatter::~JSONFormatter()
{
   // TODO Auto-generated destructor stub
}

} /* namespace FunctionFanout */
