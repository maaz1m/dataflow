// 15-745 S18 Assignment 2: available.cpp
// Group:
////////////////////////////////////////////////////////////////////////////////

#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/raw_ostream.h"

#include "dataflow.h"
#include "available-support.h"

using namespace llvm;
using namespace std;

namespace {
  class AvailableExpressions : public FunctionPass {
    
  public:
    static char ID;
    
    AvailableExpressions() : FunctionPass(ID) { }
    
    virtual bool runOnFunction(Function& F) {
      
      // Here's some code to familarize you with the Expression
      // class and pretty printing code we've provided:
      int count_bb =0;
      vector<Expression> expressions;
      for (Function::iterator FI = F.begin(), FE = F.end(); FI != FE; ++FI) {
      	BasicBlock* block = &*FI;
      	count_bb++;
        int count_instruction =0;
        for (BasicBlock::iterator i = block->begin(), e = block->end(); i!=e; ++i) {
      	  Instruction* I = &*i;
          count_instruction++;
          if (I->getOpcode() == 31) // store opcode
          {
            outs()<<*I<<" : block:"<<count_bb<<" I_count:"<<count_instruction<<" \n";
          }
      	  // outs()<<*I<<" opcode:"<<I->getOpcode()<<"\n";
          // We only care about available expressions for BinaryOperators
      	  if (BinaryOperator *BI = dyn_cast<BinaryOperator>(I)) {
      	    // Create a new Expression to capture the RHS of the BinaryOperator
      	    expressions.push_back(Expression(BI));
      	  }
      	}
        // outs()<<"i_count: "<<count_instruction<<" for bb:"<<count_bb<<"\n";
      }
      
      // Print out the expressions used in the function
      // outs()<<"count of bb: "<<count_bb<<"\n";
      // outs() << "Expressions used by this function:";
      outs() << F.getName() <<"\n";
      printSet(&expressions);

      
      // Did not modify the incoming Function.
      return false;
    }
    
    virtual void getAnalysisUsage(AnalysisUsage& AU) const {
      AU.setPreservesAll();
    }
    
  private:
  };
  
  char AvailableExpressions::ID = 0;
  RegisterPass<AvailableExpressions> X("available",
				       "15745 Available Expressions");
}
