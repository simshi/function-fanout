#ifndef JSONFORMATTER_H_
#define JSONFORMATTER_H_

namespace FunctionFanout {

class JSONFormatter
{
public:
   JSONFormatter(llvm::raw_ostream&);
   virtual ~JSONFormatter();
private:
   llvm::raw_ostream& ost_;
};

} /* namespace FunctionFanout */
#endif /* JSONFORMATTER_H_ */
