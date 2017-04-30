
#include "nodes/variable.h"
#include "core/initializer.h"

#include <string>
#include <iostream>

#include "core/common_cu.h"

#include <opencv2/opencv.hpp>

#include <glog/logging.h>

Variable::Variable(std::shared_ptr<Initializer> initializer, const NodeParam &param) : Node(param) {
	LOG_IF(FATAL, param.has_variable_param() == false) << "param.has_variable_param() == false";
	_initializer = initializer;			
}

void Variable::initForward() {	
	_initializer->init();
	_outputs[0]->initValue(_initializer->dims());
	LOG(INFO) << "Initializing Variable " << _name << " - " << _outputs[0]->value()->shape();
	if (_param.variable_param().has_weights()) {
		DF_CUDA_CHECK(cudaMemcpy(_outputs[0]->value()->mutableData(), _param.variable_param().weights().weight().data(), _outputs[0]->value()->sizeInBytes(), cudaMemcpyHostToDevice));
	}
	else if (_initializer->param().has_init_data()) {
		DF_CUDA_CHECK(cudaMemcpy(_outputs[0]->value()->mutableData(), _initializer->param().init_data().weight().data(), _outputs[0]->value()->sizeInBytes(), cudaMemcpyHostToDevice));
	}
	else {
		_initializer->apply(this);
		for (int i = 0; i < _outputs[0]->value()->size(); ++i)
			_param.mutable_variable_param()->mutable_init_param()->mutable_init_data()->add_weight(0);
		DF_CUDA_CHECK(cudaMemcpy(_param.mutable_variable_param()->mutable_init_param()->mutable_init_data()->mutable_weight()->mutable_data(),_outputs[0]->value()->data(),_outputs[0]->value()->sizeInBytes(), cudaMemcpyDeviceToHost));
	}
}

void Variable::initBackward() {	
	_outputs[0]->initDiff();
}

void Variable::transferDataToParam() {
	auto dma = _param.mutable_variable_param()->mutable_weights();
	for (int i = 0; i < _outputs[0]->value()->size(); ++i)
		dma->add_weight(0);
	DF_CUDA_CHECK(cudaMemcpy(dma->mutable_weight()->mutable_data(), _outputs[0]->value()->data(), _outputs[0]->value()->sizeInBytes(), cudaMemcpyDeviceToHost));
}

std::string Variable::to_cpp() const
{	
	std::string cpp = "auto " + _name + " = df.variable(" + _initializer->to_cpp() + ", ";	
	if (_param.variable_param().solver_name().empty())
		cpp += "NULL, ";
	else
		cpp += _param.variable_param().solver_name() + ", ";
	cpp += "\"" + _name + "\", ";
	cpp += "{" + _to_cpp_phases() + "});";
	return cpp;
}