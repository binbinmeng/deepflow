#pragma once


#include "core/node.h"

class DeepFlowDllExport Print : public Node {
public:
	enum PrintTime {
		EveryPass = 0,
		EndOfEpoch = 1
	};
	Print(const NodeParam &param);	
	int minNumInputs();
	int minNumOutputs() { return 0; }
	void initForward();
	void initBackward();
	void forward();
	void backward();
private:
	int _num_inputs = 0;
	std::string _raw_message;
	PrintTime _print_time;
};