#include "core/common_cu.h"

#include "initializers/index_fill.h"
#include "nodes/variable.h"

__global__
void IndexFillKernel(const int n, float *a,const float offset)
{
	int i = blockIdx.x*blockDim.x + threadIdx.x;
	if (i < n) a[i] = offset + i;
}

IndexFill::IndexFill(deepflow::InitParam *param) : Initializer(param) {
	LOG_IF(FATAL, param->has_index_fill_param() == false) << "param.has_index_fill_param() == false";	
}

void IndexFill::apply(Node *node) {
	auto size = node->output(0)->value()->size();
	float offset = _param->index_fill_param().offset();
	DF_KERNEL_CHECK();
	for (auto output : node->outputs()) {		
		IndexFillKernel << <numOfBlocks(size), maxThreadsPerBlock >> >(size, (float*)output->value()->gpu_data(), offset);		
		DF_KERNEL_CHECK();
	}

}

std::string IndexFill::to_cpp() const
{
	std::string cpp = "df.index_fill(";
	cpp += "{" + std::to_string(_dims[0]) + ", " + std::to_string(_dims[1]) + ", " + std::to_string(_dims[2]) + ", " + std::to_string(_dims[3]) + "}, ";
	float offset = _param->index_fill_param().offset();
	cpp += std::to_string(offset) + ")";
	return cpp;
}
