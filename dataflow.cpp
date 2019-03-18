// 15-745 S18 Assignment 2: aataflow.cpp
// Group:
////////////////////////////////////////////////////////////////////////////////

#include "dataflow.h"

namespace llvm {

	Framework::Framework(){
		direction = 0;
	}

	Framework::Framework(Function &F , BitVector init, bool dir, BitVector(*mf)(std::vector<BitVector> v), BitVector (*tf)(BitVector input,std::vector<void*> domain, BasicBlock *ptr,std::map<BasicBlock*, block> &state)){
		state.clear();
		for(Function::iterator FI = F.begin(),FE = F.end(); FI!=FE;++FI){
			BasicBlock* b = &*FI;
			state[b] = {init, init}; //map populate
		}
		//map has been initialised
		meet_function = mf;
		transfer_function = tf;
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
				state[block].IN = boundary;
			}	
			int count = 0;
			while(converge== false){
				converge =true;
				//if either sets a bitvector then make converge = false
				count++;
				for(Function::iterator FI = func.begin(),FE = func.end(); FI!=FE;++FI){
					BasicBlock* block = &*FI;
					std::vector<BitVector> v;
					if (block == &*func.begin())
					{
						v.push_back(boundary);
					}
					for (pred_iterator pit = pred_begin(block), pet = pred_end(block); pit != pet; ++pit){ //enlist all pred
						BasicBlock* b = *pit;
						if(b)
						{
							v.push_back(state[b].OUT);
						}
					}
					state[block].IN = meet_function(v); // do in 
					BitVector temp = transfer_function(state[block].IN,domain , block,state); //transform
					if (temp == state[block].OUT){continue;} //check transform
					converge = false; //if transforms then has not converged
					// print_bitvec(state[block].IN);
					// print_bitvec(state[block].OUT);
					// print_bitvec(boundary);
					state[block].OUT = temp;
				}
			printf("\nIteration #%d is complete...\n\n",count);

			}
			
		}
		else{
			bool converge = false;
			//initialise the domain and lattice
			if(BasicBlock* block = dyn_cast<BasicBlock>((--func.end())))
			{
				state[block].IN = boundary;

			}	
			int count = 0;
			while(converge== false){
				converge =true;
				//if either sets a bitvector then make converge = false
				count++;
				for(auto FI = func.getBasicBlockList().rbegin(),FE = func.getBasicBlockList().rend(); FE!=FI;++FI)
				{
					BasicBlock* block = &*FI;
					if(!block){continue;}
					std::vector<BitVector> v;
					if (block == &*(func.getBasicBlockList().rbegin()))
					{
						v.push_back(boundary);
					}
					for (succ_iterator sit = succ_begin(block), set = succ_end(block); sit != set; ++sit){ //enlist all pred
						BasicBlock* b = *sit;
						if(b)
						{
							v.push_back(state[b].OUT);
						}
					}
					
					state[block].IN = meet_function(v); // do in 

					BitVector temp = transfer_function(state[block].IN , domain , block,state); //transform
					if (temp == state[block].OUT){
						continue;
					} //check transform
					converge = false; //if transforms then has not converged
					state[block].OUT = temp;

				}
				printf("\nIteration #%d is complete...\n\n",count);


			}
	
		}
		
		//below is the code for forward direction

		
		
	}
	void Framework::print_bitvec(BitVector temp){
		for (int i = 0; i < temp.size(); ++i)
		{
			if (temp[i]==0)
			{
				printf("0");
			}
			else
			{
				printf("1");
			}
		}
		printf("-\n");
		return;
	}
  // Add code for your dataflow abstraction here.
}
