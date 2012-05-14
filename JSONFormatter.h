#ifndef JSONFORMATTER_H_
#define JSONFORMATTER_H_

namespace FunctionFanout {

class JSONFormatter
{
public:
   JSONFormatter(llvm::raw_ostream*);
   virtual ~JSONFormatter();
   virtual void BeginSourceFile();
   virtual void EndSourceFile();
private:
   llvm::raw_ostream& ost_;
};

} /* namespace FunctionFanout */
#endif /* JSONFORMATTER_H_ */
