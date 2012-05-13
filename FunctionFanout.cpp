// Copyright (c) 2012 Shi Yuanmin.Simon. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file.

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Index/CallGraph.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Path.h"

using namespace clang;

namespace FunctionFanout {
void print_function_decl(const clang::FunctionDecl* FD, llvm::raw_fd_ostream& ost)
{
   ost << "\"" << FD->getResultType().getAsString() << " ";
//   if (const CXXMethodDecl *MD = dyn_cast<CXXMethodDecl>(FD)) {
//      ost << MD->getParent()->getNameAsString() << "::";
//   }
   ost << FD->getQualifiedNameAsString() << "(";
   unsigned pv_count = 0;
   for (FunctionDecl::param_const_iterator it = FD->param_begin(); it != FD->param_end(); ++it) {
      if (pv_count++ > 0) {
         ost << ", ";
      }
      ost << (*it)->getOriginalType().getAsString();
   }
   ost << ")\"\n";
}

class FunctionBodyRecursiveASTVistor: public clang::RecursiveASTVisitor<FunctionBodyRecursiveASTVistor>
{
public:
   FunctionBodyRecursiveASTVistor(llvm::raw_fd_ostream&);
   virtual bool VisitCallExpr(clang::CallExpr*);
private:
   llvm::raw_fd_ostream& ost_;
};

FunctionBodyRecursiveASTVistor::FunctionBodyRecursiveASTVistor(llvm::raw_fd_ostream& ost) :
            ost_(ost)
{
}

bool FunctionBodyRecursiveASTVistor::VisitCallExpr(clang::CallExpr* expr)
{
   const clang::FunctionDecl* callee = expr->getDirectCallee();
   if (!callee) return true;

   print_function_decl(callee, ost_);

   return true;
}

class FunctionFanoutConsumer: public ASTConsumer
{
private:
   llvm::OwningPtr<llvm::raw_fd_ostream> output_;
   //FunctionBodyRecursiveASTVistor bodyVistor_;
   CompilerInstance& CI_;
public:
   FunctionFanoutConsumer(CompilerInstance &CI) :
               CI_(CI)
   {
      //llvm::errs() << "output_file:" << CI.getFrontendOpts().OutputFile << "\n";
      std::string ErrMsg;
      output_.reset(CI.createOutputFile("", ErrMsg, true, true, CI.getFrontendOpts().OutputFile, "fanout"));
      if (!ErrMsg.empty()) llvm::errs() << "output file creation failed!\n" << ErrMsg << "\n";
   }

   virtual ~FunctionFanoutConsumer()
   {
      output_->close();
   }

   virtual void HandleTopLevelDecl(DeclGroupRef DG)
   {
      FunctionBodyRecursiveASTVistor bodyVistor_(*output_);
      for (DeclGroupRef::iterator i = DG.begin(), e = DG.end(); i != e; ++i) {
         const Decl *D = *i;
         if (const FunctionDecl *FD = dyn_cast<FunctionDecl>(D)) {
            if (!FD->hasBody()) continue;
            SourceLocation loc = FD->getLocation();
            SourceManager& sm = CI_.getSourceManager();
            if (sm.isInSystemHeader(loc)) {
               //llvm::errs() << "name:" << FD->getQualifiedNameAsString() << ":";
               //loc.print(llvm::errs(), CI_.getSourceManager());
               continue;
            }
            print_function_decl(FD, *output_);
            *output_ << ":[\n";
            bodyVistor_.TraverseStmt(FD->getBody());
            *output_ << "],\n";
         }
      }
      output_->flush();
   }
}
;

class ASTAction: public PluginASTAction
{
public:
   virtual ~ASTAction()
   {
   }

protected:
   ASTConsumer *CreateASTConsumer(CompilerInstance &CI, llvm::StringRef)
   {
      return new FunctionFanoutConsumer(CI);
   }

   bool ParseArgs(const CompilerInstance &CI, const std::vector<std::string>& args)
   {
      for (unsigned i = 0, e = args.size(); i != e; ++i) {
         llvm::errs() << "FunctionFanout arg = " << args[i] << "\n";

         // Example error handling.
         if (args[i] == "-an-error") {
            DiagnosticsEngine &D = CI.getDiagnostics();
            unsigned DiagID = D.getCustomDiagID(DiagnosticsEngine::Error, "invalid argument '" + args[i] + "'");
            D.Report(DiagID);
            return false;
         }
      }
      if (args.size() && args[0] == "help") PrintHelp(llvm::errs());

      return true;
   }
   void PrintHelp(llvm::raw_ostream& ros)
   {
      ros << "Help for FunctionFanout plugin goes here\n";
   }

};

}

static FrontendPluginRegistry::Add<FunctionFanout::ASTAction> X("func-fanout", "collect function fanout");
