#pragma once

#include "core/export.h"
#include "core/node.h"

class DeepFlowDllExport RandomSelector : public Node {
public:
	RandomSelector(deepflow::NodeParam *param);
	int minNumInputs() { return 2; }
	int minNumOutputs() { return 1; }
	std::string op_name() const override { return "random_selector"; }
	void init();	
	void forward();
	void backward();
	std::string to_cpp() const;
private:
	float _probability;
	int _selection;
};

