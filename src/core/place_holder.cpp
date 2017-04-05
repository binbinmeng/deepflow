#include "core/place_holder.h"

PlaceHolder::PlaceHolder(NodeParam param) : Node(param) {
	LOG_IF(FATAL, param.has_place_holder_param() == false) << "param.has_place_holder_param() == false";
}

void PlaceHolder::initForward() {
		
	std::array<int, 4> dims;
	const TensorParam &tensorParam = _param.tensor_param();
	switch (tensorParam.dims_size()) {
	case 1:
		dims = { 1,tensorParam.dims(0),1,1 };
		break;
	case 2:
		dims = { tensorParam.dims(0),tensorParam.dims(1),1,1 };
		break;
	case 3:
		dims = { tensorParam.dims(0), 1, tensorParam.dims(1),tensorParam.dims(2) };
		break;
	case 4:
		dims = { tensorParam.dims(0),tensorParam.dims(1),tensorParam.dims(2),tensorParam.dims(3) };
		break;
	default:
		LOG(FATAL) << "Unsupported shape.";
	}
	Tensor::TensorType type = (Tensor::TensorType) tensorParam.type();
	_outputs[0]->initValue(dims,type);
	LOG(INFO) << "Initialize PlaceHolder (name: " << _name << ") - " << _outputs[0]->value()->toString();

}

void PlaceHolder::initBackward() {
	_outputs[0]->initDiff();
}

void PlaceHolder::forward() {
	if (_inputs[0] && _inputs[0]->connectedNode()) {
		LOG_IF(FATAL, _outputs[0]->value()->sizeInBytes() != _inputs[0]->value()->sizeInBytes()) << "Size mismatch.";
		LOG_IF(FATAL, cudaMemcpy(_outputs[0]->value()->mutableData(), _inputs[0]->value()->data(), _inputs[0]->value()->sizeInBytes(), cudaMemcpyDeviceToDevice) != 0) << "cudaMemcpy [FAILED]";
	}
}

void PlaceHolder::backward() {
	if (_inputs[0] && _inputs[0]->connectedNode()) {
		LOG_IF(FATAL, _outputs[0]->diff()->sizeInBytes() != _inputs[0]->diff()->sizeInBytes()) << "Size mismatch.";
		LOG_IF(FATAL, cudaMemcpy(_inputs[0]->diff()->mutableData(), _outputs[0]->value()->data(), _inputs[0]->value()->sizeInBytes(), cudaMemcpyDeviceToDevice) != 0) << "cudaMemcpy [FAILED]";
	}
}