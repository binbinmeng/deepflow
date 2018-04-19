#include "core/common_cu.h"

#include "nodes/bias_add.h"

BiasAdd::BiasAdd(deepflow::NodeParam *param) : Node(param) {
	LOG_IF(FATAL, param->has_bias_add_param() == false) << "param.has_bias_add_param() == false";
}

void BiasAdd::init() {
	auto inputDim = _inputs[0]->dims();
	_inner_dim = inputDim[2] * inputDim[3];
	auto weightDim = _inputs[1]->dims();
	LOG_IF(FATAL, inputDim[1] != weightDim[1]) << _name << "Bias channels between input and bias weights must be the same.";
	LOG_IF(FATAL, weightDim[0] != 1 || weightDim[2] != 1 || weightDim[3] != 1) << _name << "All bias weight dimentions must be one except channels.";
	_bias_dim = weightDim[1];	
	_outputs[0]->initValue(inputDim, _inputs[0]->value());
	_outputs[0]->initDiff(inputDim, _inputs[0]->diff());
	DF_NODE_CUDNN_CHECK(cudnnCreate(&_cudnnHandle));
}

void BiasAdd::forward() {	
	cudnnAddTensor(_cudnnHandle, &one, _inputs[1]->value()->descriptor(), _inputs[1]->value()->data(), &one, _outputs[0]->value()->descriptor(), _outputs[0]->value()->mutableData());
}

void BiasAdd::backward() {
	if (_inputs[1]->diff()) {		
		cudnnConvolutionBackwardBias(_cudnnHandle, &one, _outputs[0]->diff()->descriptor(), _outputs[0]->diff()->data(), &zero, _inputs[1]->diff()->descriptor(), _inputs[1]->diff()->mutableData());
	}
}

std::string BiasAdd::to_cpp() const
{
	std::string cpp = "auto " + _name + " = df.bias_add(" + _input_name_for_cpp(0) + ", " + _input_name_for_cpp(1) + ", ";
	cpp += "\"" + _name + "\");";	
	return cpp;
}
