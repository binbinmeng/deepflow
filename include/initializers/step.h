#pragma once

#include "core/export.h"
#include "core/initializer.h"

class DeepFlowDllExport Step : public Initializer {
public:
	Step(deepflow::InitParam *param);
	void apply(Node *node);
	void init();
	std::string to_cpp() const;
};