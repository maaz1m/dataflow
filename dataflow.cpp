// 15-745 S18 Assignment 2: aataflow.cpp
// Group:
////////////////////////////////////////////////////////////////////////////////

#include "dataflow.h"

namespace llvm {

	Framework::Framework(){
		direction = 0;
	}

	Framework::Framework(Function &F , BitVector init, bool dir, BitVector(*mf)(std::vector<BitVector> v), BitVector (*tf)(BitVector input,std::vector<void*> domain, BasicBlock *ptr)){
		referal.clear();
		for(Function::iterator FI = F.begin(),FE = F.end(); FI!=FE;++FI){
			BasicBlock* b = &*FI;
			referal[b] = {init, init}; //map populate
		}
		//map has been initialised
		meet_function = mf;
		transform_function = tf;
		//both functions has been initialised
		direction = dir; // 0 means downwards and 1 means upwards
		
	}


	bool Framework::getDirection(){
		return direction;
	}
	void Framework::setDirection(bool dir){
		direction = dir;
		return;
	}
	
	void Framework::runAnalysis(Function &func, std::vector<void*> domain, BitVector boundary){
		if(!direction){
			bool converge = false;

			//initialise the domain and lattice
			if(BasicBlock* block = dyn_cast<BasicBlock>(func.begin()))
			{
				referal[block].IN = boundary; 	
			}	
			while(converge== false){
				converge =true;
				//if either sets a bitvector then make converge = false
				for(Function::iterator FI = func.begin(),FE = func.end(); FI!=FE;++FI){
					BasicBlock* block = &*FI;
					std::vector<BitVector> v;
					for (pred_iterator pit = pred_begin(block), pet = pred_end(block); pit != pet; ++pit){ //enlist all pred
						BasicBlock* b = *pit;
						if(b)
						{
							v.push_back(referal[b].OUT);
						}
					}
					referal[block].IN = meet_function(v); // do in 
					BitVector temp = transform_function(referal[block].IN,domain , block); //transform
					if (temp == referal[block].IN){continue;} //check transform
					converge = false; //if transforms then has not converged
					referal[block].OUT = temp;
				}

			}
			
		}
		else{
			bool converge = false;

			//initialise the domain and lattice
			if(BasicBlock* block = dyn_cast<BasicBlock>(func.end()))
			{
				referal[block].IN = boundary; 	
			}	
			while(converge== false){
				converge =true;
				//if either sets a bitvector then make converge = false
				for(Function::iterator FI = func.begin(),FE = func.end(); FI!=FE;--FE){
					BasicBlock* block = &*FE;
					std::vector<BitVector> v;
					for (succ_iterator sit = succ_begin(block), set = succ_end(block); sit != set; ++sit){ //enlist all pred
						BasicBlock* b = *sit;
						if(b)
						{
							v.push_back(referal[b].OUT);
						}
					}
					referal[block].IN = meet_function(v); // do in 
					BitVector temp = transform_function(referal[block].IN,domain , block); //transform
					if (temp == referal[block].IN){continue;} //check transform
					converge = false; //if transforms then has not converged
					referal[block].OUT = temp;
				}

			}
		}
		
		//below is the code for forward direction

		
		
	}
  // Add code for your dataflow abstraction here.
}
