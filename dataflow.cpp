// 15-745 S18 Assignment 2: aataflow.cpp
// Group:
////////////////////////////////////////////////////////////////////////////////

#include "dataflow.h"

namespace llvm {

	Framework::Framework(){
		direction = 0;
	}

	bool Framework::getDirection(){
		return direction;
	}
	void Framework::setDirection(bool dir){
		direction = dir;
		return;
	}
	BitVector Framework::getDomain(){
		return domain;
	}
  // Add code for your dataflow abstraction here.
}
