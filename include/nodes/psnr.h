#pragma once


#include "core/node.h"

class DeepFlowDllExport Psnr : public Node {
public:
	Psnr(deepflow::NodeParam *param);
	int minNumInputs() { return 2; }
	int minNumOutputs() { return 0; }
	std::string op_name() const override { return "psnr"; }
	void init();	
	void forward();
	void backward();
	std::string to_cpp() const;
private:
	deepflow::ActionTime _print_time;
	float *d_square_error;
	float *d_sum_square_error;
	cudnnHandle_t _cudnnHandle;	
	cudnnReduceTensorDescriptor_t _reduce_tensor_desciptor;
	cudnnTensorDescriptor_t _output_desc;
	float *_d_workspace;
	size_t _workspaceSizeInBytes;
};
