// 15-745 S18 Assignment 2: dataflow.h
// Group:
////////////////////////////////////////////////////////////////////////////////

#ifndef __CLASSICAL_DATAFLOW_H__
#define __CLASSICAL_DATAFLOW_H__

#include <stdio.h>
#include <iostream>
#include <queue>
#include <vector>

#include "llvm/IR/Instructions.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/ValueMap.h"
#include "llvm/IR/CFG.h"

namespace llvm {
	class Framework{
	public:
		//entry basic block
		//exit basic block
		
		BitVector domain;
		bool direction; // 0 means downwards and 1 means upwards

		Framework();
		bool getDirection();
		BitVector getDomain();
		void setDirection(bool dir);
	};

// Add definitions (and code, depending on your strategy) for your dataflow
// abstraction here.

}

#endif
