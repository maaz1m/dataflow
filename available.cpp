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

	//Helper function that finds the index of expression in domain
	int findExpression(Expression* e, std::vector<void*> domain)
	{
		for (unsigned i = 0; i < domain.size(); ++i)
		{
			if(*e == *((Expression*)domain[i]))
			{
				return i;
			}
		}
		return -1;
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
			result &= *i;
		}
	
		// else if(meet_operator=="union")
		// {
		// 	for (std::vector<BitVector>::iterator i = inputs.begin(); i != inputs.end(); ++i)
		// 	{
		// 		result |= *i;
		// 	}
		// }
		return result;
	}

	void print_all (Function &F , std::map<BasicBlock*,block>referal,std::vector<void*> domain){
		for (Function::iterator b = F.begin(); b!=F.end() ; ++b)
		{
			BasicBlock* bl = &*b;
			BitVector out = referal[bl].OUT;
			std::vector<Expression> x;
			for (int i = 0; i < out.size(); ++i)
			{
				if (out[i]==1)
				{
					Expression* currExp = (Expression*)(domain[i]);
					x.push_back(*currExp);
				}
			}
			printf("%d\n", x.size());
			printSet(&x);
			x.clear();
		}
		outs()<<"all printing done\n";
	}

	BitVector transferFunction(BitVector input, std::vector<void*> domain, BasicBlock* block)
	{
		int domainSize = domain.size();
		BitVector Gen(domainSize);
		BitVector Kill(domainSize);

		for (BasicBlock::iterator i = block->begin(), e = block->end(); i!=e; ++i)
		{
			Instruction* instruction_ptr = &*i;
			if (BinaryOperator *binary_instruction = dyn_cast<BinaryOperator>(instruction_ptr))
			{
				Expression* expression_ptr = new Expression(binary_instruction); //RHS of operator
				int index = findExpression(expression_ptr, domain);
				if(index != -1){
					Gen.set(index);
				}

				//Kill expressions in which LHS is an operand
				StringRef instruction_name  =  i->getName();
				
				if(!instruction_name.empty())
				{

					for(auto d = domain.begin(); d != domain.end() ; d++)
					{
						Expression* currExp = (Expression*)(*d);

						StringRef op1 = currExp->v1->getName();
						StringRef op2 = currExp->v2->getName();

						if(op1.equals(instruction_name) || op2.equals(instruction_name))
						{
								// Kill if either operand 1 or 2 match the variable assigned
								int index = findExpression(currExp, domain);
								if (index != -1)
								{
									Kill.set(index);
									Gen.reset(index);
								}
						}
					}
				}
			}
		}
		BitVector output = Kill.flip();
		output &= input;
		output |= Gen;
		return output;
	}
	class AvailableExpressions : public FunctionPass {
	public:
		static char ID;
		AvailableExpressions() : FunctionPass(ID) { }
		virtual bool runOnFunction(Function& F) {

			//Declare domain
			std::vector<void*> domain;

			//Initialize domain
			for(Function::iterator b = F.begin(); b != F.end(); ++b)
			{
				BasicBlock* block = &*b;
				for (BasicBlock::iterator i = block->begin(); i != block->end(); ++i)
				{
					Instruction* instruction_ptr = &*i;
					if(BinaryOperator* binary_instruction = dyn_cast<BinaryOperator>(instruction_ptr))
					{
						Expression* expression_ptr = new Expression(binary_instruction);
						int found = findExpression(expression_ptr, domain);
						if(found==-1)
							domain.push_back(expression_ptr);
					}					
				}
			}

			for(void* element: domain)
			{
				outs() << "Expression: " << ((Expression*)element)->toString() << '\n';
			}
			outs()<<"Domain done \n";

			BitVector boundary(domain.size(), 1);
			BitVector init(domain.size(), 0);
			BitVector(*mf)(std::vector<BitVector> v);
			mf = &meet;
			BitVector (*tf)(BitVector input,std::vector<void*> domain, BasicBlock *ptr);
			tf = &transferFunction;
			Framework pass(F,init,0,mf,tf);
			outs()<<"Frameword init \n";
			pass.runAnalysis(F, domain, boundary);
			outs()<<"analysis done \n";
			print_all(F,pass.referal,domain);
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
