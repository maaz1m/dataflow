// 15-745 S18 Assignment 2: dataflow.h
// Group:
////////////////////////////////////////////////////////////////////////////////

#ifndef __CLASSICAL_DATAFLOW_H__
#define __CLASSICAL_DATAFLOW_H__

#include <stdio.h>
#include <iostream>
#include <queue>
#include <vector>
#include <map> 

#include "llvm/IR/Instructions.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/ValueMap.h"
#include "llvm/IR/CFG.h"

namespace llvm {
	struct block
	{
		BitVector IN;
		BitVector OUT;
	};

	class Framework{
	public:

		
		std::map<BasicBlock*, block> state;
		BitVector (*meet_function)(std::vector<BitVector> v); // meet function
		//the state in the tranfer function is for PHI node catering
		BitVector (*transfer_function)(BitVector input, std::vector<void*> domain, BasicBlock *ptr,std::map<BasicBlock*, block> &state); //gives OUT bitvector
		bool direction; // 0 means downwards and 1 means upwards
		Framework();
		Framework(Function &F,BitVector init, bool dir, BitVector(*mf)(std::vector<BitVector> v), BitVector (*tf)(BitVector input,std::vector<void*> domain, BasicBlock *ptr,std::map<BasicBlock*, block> &state));
		// init dir mf tf
		bool getDirection();
		void setDirection(bool dir);
		void runAnalysis(Function &func, std::vector<void*> domain, BitVector boundary);
		void print_bitvec(BitVector temp);
	};
	

// Add definitions (and code, depending on your strategy) for your dataflow
// abstraction here.

}

#endif
