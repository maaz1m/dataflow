// 15-745 S18 Assignment 2: liveness.cpp
// Group:
////////////////////////////////////////////////////////////////////////////////

#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

#include "dataflow.h"

using namespace llvm;

namespace {
  //Helper function that finds the index of expression in domain
  int findVal(Value* v, std::vector<void*> domain)
  {
    for (unsigned i = 0; i < domain.size(); ++i)
    {
      if(v == ((Value*)domain[i]))
      {
        return i;
      }
    }
    return -1;
  }

  BitVector transferFunction(BitVector input, std::vector<void*> domain, BasicBlock* block)
  {
    //Write transfer function here
  }

  BitVector meet(std::vector<BitVector> inputs)
  {
    if (inputs.size()==0)
    {
      BitVector temp;
      temp.clear();
      return temp;
    }
    BitVector result = inputs[0];
     for (std::vector<BitVector>::iterator i = inputs.begin(); i != inputs.end(); ++i)
     {
       result |= *i;
     }
    return result;
  }

  class Liveness : public FunctionPass {
  public:
    static char ID;

    Liveness() : FunctionPass(ID) { }

    virtual bool runOnFunction(Function& F) {
      std::vector<void*> domain;

      //Initialize domain
      for(Function::iterator b = F.begin(); b != F.end(); ++b)
      {
        BasicBlock* block = &*b;
        for (BasicBlock::iterator i = block->begin(); i != block->end(); ++i)
        {
          Instruction* instruction_ptr = &*i;
          for(User::op_iterator OI = instruction_ptr->op_begin();OI!= instruction_ptr->op_end();++OI)  
          {
            Value *val = *OI;
            if(isa<Instruction>(val) || isa<Argument>(val))
            {
              int ind =findVal(val,domain);
              if(ind == -1)
              {
                domain.push_back(val);
              }
            }
            if( isa<Instruction>(val))
            {
              outs()<<"ins: "<<val->getName()<<"\n";
            }
            if(isa<Argument>(val))
            {
              outs()<<"arg: "<<val->getName()<<"\n";
            }
          }         
        }
      }

      for(void* element: domain)
      {
        outs() << "Variables: " << *((Value*)element)<< '\n';
      }
      outs()<<"Domain done \n";
      BitVector boundary(domain.size(), 1); // FIX THIS <---------------
      BitVector init(domain.size(), 0); // FIX THIS <---------------
      BitVector(*mf)(std::vector<BitVector> v);
      mf = &meet;
      BitVector (*tf)(BitVector input,std::vector<void*> domain, BasicBlock *ptr);
      tf = &transferFunction;
      // Framework pass(F,init,1,mf,tf);
      // outs()<<"Frameword init \n";
      // pass.runAnalysis(F, domain, boundary);
      // outs()<<"analysis done \n";
      // Did not modify the incoming Function.

      return false;
    }

    virtual void getAnalysisUsage(AnalysisUsage& AU) const {
      AU.setPreservesAll();
    }

  private:
  };

  char Liveness::ID = 0;
  RegisterPass<Liveness> X("liveness", "15745 Liveness");
}
