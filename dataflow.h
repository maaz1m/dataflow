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
		block(){
			IN.clear();
			OUT.clear();
		}
	};

	class Framework{
	public:
		//entry basic block
		//exit basic block
		Function *func;
		std::map<BasicBlock*, block> referal;
		BitVector (*meet_function)(std::vector<BitVector> v); // meet function
		BitVector (*transform_function)(BitVector input, BasicBlock *ptr); //gives OUT bitvector
		BitVector domain; //
		bool direction; // 0 means downwards and 1 means upwards
		Framework();
		Framework(Function F,BitVector init, bool dir, BitVector(*mf)(std::vector<BitVector> v), BitVector (*tf)(BitVector input, BasicBlock *ptr));

		bool getDirection();
		BitVector getDomain();
		void setDirection(bool dir);
		void runAnalysis();
	};

// Add definitions (and code, depending on your strategy) for your dataflow
// abstraction here.

}

#endif
