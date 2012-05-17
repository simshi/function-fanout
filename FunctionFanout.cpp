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

#include "JSONFormatter.h"

using namespace clang;

namespace FunctionFanout {
void print_function_decl(const clang::FunctionDecl& FD, llvm::raw_ostream* ost)
{
   (*ost) << "\"" << FD.getResultType().getAsString() << " ";
   (*ost) << FD.getQualifiedNameAsString() << "(";

   unsigned pv_count = 0;
   for (FunctionDecl::param_const_iterator it = FD.param_begin(); it != FD.param_end(); ++it) {
      if (pv_count++ > 0) {
         (*ost) << ", ";
      }
      (*ost) << (*it)->getOriginalType().getAsString();
   }

   (*ost) << ")\"";
}

class FunctionBodyRecursiveASTVistor: public clang::RecursiveASTVisitor<FunctionBodyRecursiveASTVistor>
{
public:
   FunctionBodyRecursiveASTVistor(llvm::raw_ostream*);
   virtual bool VisitCallExpr(clang::CallExpr*);
private:
   llvm::raw_ostream& ost_;
};

FunctionBodyRecursiveASTVistor::FunctionBodyRecursiveASTVistor(llvm::raw_ostream* ost) :
            ost_(*ost)
{
}

bool FunctionBodyRecursiveASTVistor::VisitCallExpr(clang::CallExpr* expr)
{
   const clang::FunctionDecl* callee = expr->getDirectCallee();
   if (!callee) return true;

   print_function_decl(*callee, &ost_);
   ost_ << ", ";

   return true;
}

class FunctionFanoutConsumer: public ASTConsumer
{
private:
   CompilerInstance& CI_;
   llvm::raw_ostream& ost_;
   JSONFormatter& fmt_;
public:
   FunctionFanoutConsumer(CompilerInstance &CI, llvm::raw_ostream* ost, JSONFormatter* fmt) :
               CI_(CI), ost_(*ost), fmt_(*fmt)
   {
   }

   virtual ~FunctionFanoutConsumer()
   {
      //llvm::errs() << __FUNCTION__ << "\n";
      ost_.flush();
      fmt_.EndSourceFile();
   }

   virtual void HandleTopLevelDecl(DeclGroupRef DG)
   {
      FunctionBodyRecursiveASTVistor bodyVistor_(&ost_);
      for (DeclGroupRef::iterator i = DG.begin(), e = DG.end(); i != e; ++i) {
         const Decl *D = *i;
         if (const FunctionDecl *FD = dyn_cast<FunctionDecl>(D)) {
            if (!FD->hasBody()) continue;
            if (CI_.getSourceManager().isInSystemHeader(FD->getLocation())) {
               //loc.print(llvm::errs(), CI_.getSourceManager());
               continue;
            }

            std::vector<std::string> vec;
            for (FunctionDecl::param_const_iterator it = FD->param_begin(); it != FD->param_end(); ++it) {
               vec.push_back((*it)->getOriginalType().getAsString());
            }
            fmt_.AddDefinition(FD->getQualifiedNameAsString(), FD->getResultType().getAsString(), vec);

            bodyVistor_.TraverseStmt(FD->getBody());

            fmt_.EndDefinition();
         }
      }

      ost_.flush();
   }
};

class ASTAction: public PluginASTAction
{
   llvm::raw_fd_ostream* output_;

   void CreateOutput(CompilerInstance& CI)
   {
      //llvm::errs() << __FUNCTION__ << ":" << CI.getFrontendOpts().OutputFile << "\n";
      if (output_) return;

      std::string errMsg;
      output_ = CI.createOutputFile("", errMsg, true, true, CI.getFrontendOpts().OutputFile, "fanout");
      if (!output_) llvm::errs() << "Failed to create output file:" << errMsg << "\n";
   }

public:
   ASTAction() :
               output_(0)
   {
   }
   virtual ~ASTAction()
   {
      //llvm::errs() << __FUNCTION__ << "\n";
   }

protected:
   ASTConsumer *CreateASTConsumer(CompilerInstance &CI, StringRef filename)
   {
      //llvm::errs() << __FUNCTION__ << ":" << filename << "\n";
      CreateOutput(CI);
      JSONFormatter* fmt = new JSONFormatter(output_);

      fmt->BeginSourceFile();
      return new FunctionFanoutConsumer(CI, output_, fmt);
   }

#if 0
   virtual bool BeginSourceFileAction(CompilerInstance& CI, StringRef filename)
   {
      llvm::errs() << __FUNCTION__ << ":" << filename << "\n";

      CreateOutput(CI);
      if (output_) {
         *output_ << "{\n";
         // Set to consumer because it could be created before this function invoked.
         if (consumer_) consumer_->SetRawOStream(output_);
         return true;
      }

      return false;
   }

   virtual void EndSourceFileAction()
   {
      llvm::errs() << __FUNCTION__ << "\n";

      *output_ << "}\n";
      output_->flush();
   }
#endif
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
