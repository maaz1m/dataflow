// 15-745 S18 Assignment 2: liveness.cpp
// Group:
////////////////////////////////////////////////////////////////////////////////

#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "available-support.h"
#include "dataflow.h"

using namespace llvm;

namespace {

  void print_val(std::vector<Value*> &y){
    if (y.size()==0)
    {
      outs()<<"{}\n";
      return;
    }
    outs()<<"{";
    for (unsigned i = 0; i < y.size()-1; ++i)
    {
      outs()<<llvm::getShortValueName(y[i])<<" ,";
    }
    outs()<<llvm::getShortValueName(y[y.size()-1])<<"}\n";
    return;
  }
  void print_all (Function &F , std::map<BasicBlock*,block>referal,std::vector<void*> domain){
    for (Function::iterator b = F.begin(); b!=F.end() ; ++b)
    {
      BasicBlock* bl = &*b;
      BitVector out = referal[bl].OUT;
      BitVector in = referal[bl].IN;
      std::vector<Value*> x;
      std::vector<Value*> y;
      
      for (unsigned i = 0; i < out.size(); ++i)
      {
        if (out[i]==1)
        {
          Value* currExp = (Value*)(domain[i]);
          x.push_back(currExp);
        }
      }
      for (unsigned i = 0; i < in.size(); ++i)
      {
        if (in[i]==1)
        {
          Value* currExp = (Value*)(domain[i]);
          y.push_back(currExp);
        }
      }
      outs()<<"OUT: ";
      print_val(x);
      outs()<<"IN: ";
      print_val(y);
      outs()<<"\n";
      

      x.clear();
      y.clear();
    }
  }

  void print_bitvec_map(BitVector in,std::vector<void*> domain ){
    std::vector<Value*> x;
    for (unsigned i = 0; i < in.size(); ++i)
      {
        if (in[i]==1)
        {
          Value* currExp = (Value*)(domain[i]);
          x.push_back(currExp);
        }
      }
    print_val(x);
  }


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

  BitVector transferFunction(BitVector input, std::vector<void*> domain, BasicBlock* block, std::map<BasicBlock*,llvm::block> &referal)
  {
    int domainSize = domain.size();
    BitVector defSet(domainSize,0);
    BitVector useSet(domainSize,0);
    BitVector result= input;
    int index;
    //remove all phi node instructions
    // outs()<<"--------------------------------------------------------------------\n";
      for (succ_iterator sit = succ_begin(block), set = succ_end(block); sit != set; ++sit){ //enlist all pred
      BasicBlock* b = *sit;
      if(b)
      {
        for (BasicBlock::reverse_iterator i = b->rbegin(); i != b->rend(); ++i) {
          // Locally exposed uses
          Instruction* instruction_ptr = &*i;
          // outs()<<*instruction_ptr<<"\n";
          // Phi nodes: add operands to the list we store in transferOutput
          if (PHINode* phi_instruction = dyn_cast<PHINode>(instruction_ptr))
          {
            for (unsigned ind = 0; ind < phi_instruction->getNumIncomingValues(); ind++)
            {
               Value* val = phi_instruction->getIncomingValue(ind); //get value from the block above
                if (isa<Instruction>(val) || isa<Argument>(val)) // if a value that we actually need
                {
                    BasicBlock* valBlock = phi_instruction->getIncomingBlock(ind); //get blcok of that very instruction
                    if(referal.find(valBlock) == referal.end())
                    {
                      continue;
                    }  
                    if (valBlock != block)
                    {
                      index = findVal(val, domain);
                      referal[valBlock].IN.reset(index);
                      result.reset(index); 
                    }
                }
            }
          }
        }
      }
    }

    for (BasicBlock::reverse_iterator i = block->rbegin(); i != block->rend(); ++i) {
        // Locally exposed uses
        Instruction* instruction_ptr = &*i;
        print_bitvec_map(result,domain);
        outs()<<*instruction_ptr<<"\n";
        // Phi nodes: add operands to the list we store in transferOutput
        if (PHINode* phi_instruction = dyn_cast<PHINode>(instruction_ptr))
        {
          //phi node instructions, need pointer to their individual blocks
          for (unsigned ind = 0; ind < phi_instruction->getNumIncomingValues(); ind++)
          {
             Value* val = phi_instruction->getIncomingValue(ind); //get value from the block above
              if (isa<Instruction>(val) || isa<Argument>(val)) // if a value that we actually need
              {
                  BasicBlock* valBlock = phi_instruction->getIncomingBlock(ind); //get blcok of that very instruction
                  // referal has no mapping for this block, then create one
                  if(referal.find(valBlock) == referal.end())
                  {
                    outs()<<"block doesn't exist\n";
                    continue;
                  }  
                  index = findVal(val, domain);
                  int index_def = findVal(instruction_ptr,domain);

                  if (index!= -1 && index!= index_def){
                      useSet.set(index);
                      result.set(index);  
                  }
              }
          }
        }

        //Non-phi nodes: Simply add operands to the use set
        else {

            for (User::op_iterator opnd = i->op_begin(), opE = i->op_end(); opnd != opE; ++opnd) {
                Value* val = *opnd;
                if (isa<Instruction>(val) || isa<Argument>(val)) {
                    index = findVal(val, domain);
                    int index_def = findVal(instruction_ptr,domain);
                    // Add to useSet only if not already defined in the block somewhere earlier
                    if (index!= -1 && index!= index_def){
                      useSet.set(index);
                      result.set(index);  
                    }
                }
            }
        }
        // Definitions
        index = findVal(instruction_ptr, domain);
        if (index != -1){
          defSet.set(index);
          result.reset(index);  
        }
        
    }

    // Transfer function = useSet U (input - defSet)

    // BitVector result = defSet.flip();
    // // input - defSet = input INTERSECTION Complement of defSet
    // result &= input;
    // result |= useSet;

    // outs()<<"--------------------------------------------------------------------\n";
    return result; 
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
          }         
        }
      }
      outs()<<"=======================================================================================\n";
      outs()<<"Following are the expressions in the entire program:\n";
      outs()<<"=======================================================================================\n";
      
      for(void* element: domain)
      {
        outs() << "Variables: " << llvm::getShortValueName(((Value*)element))<< '\n';
      }
      outs()<<"=======================================================================================\n";
      outs()<<"Running Analysis now:\n";
      outs()<<"=======================================================================================\n";
      BitVector boundary(domain.size(), 0); 
      BitVector init(domain.size(), 0); 
      BitVector(*mf)(std::vector<BitVector> v);
      mf = &meet;
      BitVector (*tf)(BitVector input,std::vector<void*> domain, BasicBlock *ptr, std::map<BasicBlock*, block> &referal);
      tf = &transferFunction;
      Framework pass(F,init,1,mf,tf);
      pass.runAnalysis(F, domain, boundary);
      outs()<<"=======================================================================================\n";
      outs()<<"Analysis Complete \n";
      outs()<<"=======================================================================================\n";
      outs()<<"\nFollowing is the final block level IN and OUT set representation after Convergence:\n";
      outs()<<"=======================================================================================\n";
      // Did not modify the incoming Function.
      print_all(F,pass.referal,domain);


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
