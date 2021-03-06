#pragma once

#include "core/export.h"
#include "core/initializer.h"
#include "proto/deepflow.pb.h"

class DeepFlowDllExport RandomUniform : public Initializer {
public:
	RandomUniform(deepflow::InitParam *param);
	void init() {}
	void apply(Node *node);
	std::string to_cpp() const;
};