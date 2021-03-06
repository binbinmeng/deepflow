#include "core/session.h"

#include "nodes/variable.h"
#include "nodes/place_holder.h"

#include "initializers/fill.h"
#include "initializers/index_fill.h"
#include "initializers/random_uniform.h"
#include "initializers/random_normal.h"
#include "initializers/step.h"
#include "initializers/three_state.h"
#include "initializers/truncated_normal.h"
#include "initializers/gradient_fill.h"
#include "initializers/constant.h"

#include "core/solver.h"

#include "solvers/sgd_solver.h"
#include "solvers/adam_solver.h"
#include "solvers/adadelta_solver.h"
#include "solvers/rmsprop_solver.h"

#include "nodes/add.h"
#include "nodes/matmul.h"
#include "nodes/leaky_relu.h"
#include "nodes/softmax.h"
#include "nodes/exp.h"
#include "nodes/abs.h"
#include "nodes/square.h"
#include "nodes/bias_add.h"
#include "nodes/dropout.h"
#include "nodes/convolution_2d.h"
#include "nodes/pooling.h"
#include "nodes/reduce.h"
#include "nodes/equal.h"
#include "nodes/display.h"
#include "nodes/transposed_conv_2d.h"
#include "nodes/square_error.h"
#include "nodes/activation.h"
#include "nodes/psnr.h"
#include "nodes/random_selector.h"
#include "nodes/block.h"
#include "nodes/restructure.h"
#include "nodes/accumulator.h"
#include "nodes/print.h"
#include "nodes/psnr.h"
#include "nodes/logger.h"
#include "nodes/image_reader.h"
#include "nodes/multiplexer.h"
#include "nodes/batch_normalization.h"
#include "nodes/dot.h"
#include "nodes/replay_memory.h"
#include "nodes/sio_output.h"
#include "nodes/loss.h"
#include "nodes/log.h"
#include "nodes/lifting.h"
#include "nodes/patching.h"
#include "nodes/reduce_all.h"
#include "nodes/image_writer.h"
#include "nodes/resize.h"
#include "nodes/split.h"
#include "nodes/switch.h"
#include "nodes/lrn.h"
#include "nodes/prelu.h"
#include "nodes/dprelu.h"
#include "nodes/concate.h"
#include "nodes/reshape.h"
#include "nodes/batch_stddev.h"
#include "nodes/pass_through.h"
#include "nodes/gaussian.h"
#include "nodes/gaussian_kernel.h"
#include "nodes/patch_sampling.h"
#include "nodes/max.h"
#include "nodes/nand.h"
#include "nodes/spatial_transformer.h"
#include "nodes/gabor_kernel.h"

#include "generators/mnist_reader.h"
#include "generators/data_generator.h"
#include "generators/image_batch_reader.h"
#include "generators/text_image_generator.h"

#include <unordered_map>
#include <map>

#include<memory>

#include <vector>

#include <fstream>

#include <chrono>

#include <ctime>

std::shared_ptr<Initializer> _create_initializer(deepflow::InitParam *init_param) {
	
	if (init_param->has_fill_param()) {
		return std::make_shared<Fill>(init_param);
	}
	else if (init_param->has_index_fill_param()) {
		return std::make_shared<IndexFill>(init_param);
	}
	else if (init_param->has_random_uniform_param()) {
		return std::make_shared<RandomUniform>(init_param);
	}
	else if (init_param->has_step_param()) {
		return std::make_shared<Step>(init_param);
	}
	else if (init_param->has_random_normal_param()) {
		return std::make_shared<RandomNormal>(init_param);
	}
	else if (init_param->has_three_state_param()) {
		return std::make_shared<ThreeState>(init_param);
	}
	else if (init_param->has_truncated_normal_param()) {
		return std::make_shared<TruncatedNormal>(init_param);
	}
	else if (init_param->has_gradient_fill_param()) {
		return std::make_shared<GradientFill>(init_param);
	}
	else if (init_param->has_constant_param()) {
		return std::make_shared<Constant>(init_param);
	}
	else {
		LOG(FATAL) << "Unsupported Initializer";
	}

	return NULL;
}

std::shared_ptr<Node> Session::_create_node(deepflow::NodeParam *node_param) {

	if (node_param->has_batch_normalization_param())
		return std::make_shared<BatchNormalization>(node_param);
	else if (node_param->has_image_batch_reader_param())
		return std::make_shared<ImageBatchReader>(node_param);
	else if (node_param->has_mnist_param())
		return std::make_shared<MNISTReader>(node_param);
	else if (node_param->has_data_generator_param()) {
		auto init_param = node_param->mutable_variable_param()->mutable_init_param();
		std::shared_ptr<Initializer> initializer = _create_initializer(init_param);
		return std::make_shared<DataGenerator>(initializer, node_param);
	}
	else if (node_param->has_text_image_generator_param()) {
		auto init_param = node_param->mutable_variable_param()->mutable_init_param();
		std::shared_ptr<Initializer> initializer = _create_initializer(init_param);
		return std::make_shared<TextImageGenerator>(initializer, node_param);
	}
	else if (node_param->has_variable_param()) {
		auto init_param = node_param->mutable_variable_param()->mutable_init_param();
		std::shared_ptr<Initializer> initializer = _create_initializer(init_param);
		return std::make_shared<Variable>(initializer, node_param);
	}
	else if (node_param->has_image_reader_param())
		return std::make_shared<ImageReader>(node_param);
	else if (node_param->has_transposed_conv_2d_param())
		return std::make_shared<TransposedConvolution2D>(node_param);
	else if (node_param->has_matmul_param())
		return std::make_shared<MatMul>(node_param);
	else if (node_param->has_conv_2d_param())
		return std::make_shared<Convolution2D>(node_param);
	else if (node_param->has_abs_param())
		return std::make_shared<Abs>(node_param);
	else if (node_param->has_log_param())
		return std::make_shared<Log>(node_param);
	else if (node_param->has_loss_param())
		return std::make_shared<Loss>(node_param);
	else if (node_param->has_max_param())
		return std::make_shared<Max>(node_param);
	else if (node_param->has_reduce_all_param())
		return std::make_shared<ReduceAll>(node_param);
	else if (node_param->has_square_error_param())
		return std::make_shared<SquareError>(node_param);
	else if (node_param->has_patching_param())
		return std::make_shared<Patching>(node_param);
	else if (node_param->has_lifting_param())
		return std::make_shared<Lifting>(node_param);
	else if (node_param->has_resize_param())
		return std::make_shared<Resize>(node_param);
	else if (node_param->has_pooling_param())
		return std::make_shared<Pooling>(node_param);
	else if (node_param->has_reduce_param())
		return std::make_shared<Reduce>(node_param);
	else if (node_param->has_display_param())
		return std::make_shared<Display>(node_param);
	else if (node_param->has_leaky_relu_param())
		return std::make_shared<LeakyRelu>(node_param);
	else if (node_param->has_dprelu_param())
		return std::make_shared<DPRelu>(node_param);
	else if (node_param->has_prelu_param())
		return std::make_shared<PRelu>(node_param);
	else if (node_param->has_softmax_param())
		return std::make_shared<Softmax>(node_param);
	else if (node_param->has_dropout_param())
		return std::make_shared<Dropout>(node_param);
	else if (node_param->has_image_writer_param())
		return std::make_shared<ImageWriter>(node_param);
	else if (node_param->has_replay_memory_param())
		return std::make_shared<ReplayMemory>(node_param);
	else if (node_param->has_add_param())
		return std::make_shared<Add>(node_param);
	else if (node_param->has_exp_param())
		return std::make_shared<Exp>(node_param);
	else if (node_param->has_bias_add_param())
		return std::make_shared<BiasAdd>(node_param);
	else if (node_param->has_equal_param())
		return std::make_shared<Equal>(node_param);
	else if (node_param->has_activation_param())
		return std::make_shared<Activation>(node_param);
	else if (node_param->has_concate_param())
		return std::make_shared<Concate>(node_param);
	else if (node_param->has_place_holder_param())
		return std::make_shared<PlaceHolder>(node_param);
	else if (node_param->has_print_param())
		return std::make_shared<Print>(node_param);
	else if (node_param->has_logger_param())
		return std::make_shared<Logger>(node_param);
	else if (node_param->has_square_param())
		return std::make_shared<Square>(node_param);
	else if (node_param->has_restructure_param())
		return std::make_shared<Restructure>(node_param);
	else if (node_param->has_psnr_param())
		return std::make_shared<Psnr>(node_param);
	else if (node_param->has_random_selector_param())
		return std::make_shared<RandomSelector>(node_param);
	else if (node_param->has_multiplexer_param())
		return std::make_shared<Multiplexer>(node_param);
	else if (node_param->has_accumulator_param())
		return std::make_shared<Accumulator>(node_param);
	else if (node_param->has_dot_param())
		return std::make_shared<Dot>(node_param);
	else if (node_param->has_sio_output_param())
		return std::make_shared<SIOOutput>(node_param);
	else if (node_param->has_split_param())
		return std::make_shared<Split>(node_param);
	else if (node_param->has_switch_param())
		return std::make_shared<Switch>(node_param);
	else if (node_param->has_reshape_param())
		return std::make_shared<Reshape>(node_param);
	else if (node_param->has_lrn_param())
		return std::make_shared<LRN>(node_param);
	else if (node_param->has_pass_through_param())
		return std::make_shared<PassThrough>(node_param);
	else if (node_param->has_batch_stddev_param())
		return std::make_shared<BatchStdDev>(node_param);
	else if (node_param->has_gaussian_kernel_param())
		return std::make_shared<ConvGaussianKernel>(node_param);
	else if (node_param->has_patch_sampling_param())
		return std::make_shared<PatchSampling>(node_param);
	else if (node_param->has_gaussian_param())
		return std::make_shared<Gaussian>(node_param);
	else if (node_param->has_nand_param())
		return std::make_shared<Nand>(node_param);
	else if (node_param->has_spatial_transformer_param())
		return std::make_shared<SpatialTransformer>(node_param);
	else if (node_param->has_gabor_kernel_param())
		return std::make_shared<GaborKernel>(node_param);
	else {
		LOG(FATAL) << "Unsupported Node";
	}

	return 0;
}

std::shared_ptr<Solver> Session::_create_solver(deepflow::SolverParam *solver_param) {
	if (solver_param->has_sgd_solver()) {
		return std::make_shared<SGDSolver>(solver_param);
	}
	else if (solver_param->has_adam_solver()) {
		return std::make_shared<AdamSolver>(solver_param);
	}
	else if (solver_param->has_adadelta_solver()) {
		return std::make_shared<AdaDeltaSolver>(solver_param);
	}
	else if (solver_param->has_rmsprop_solver()) {
		return std::make_shared<RMSPropSolver>(solver_param);
	}
	else {
		LOG(FATAL) << "Unsupported Solver";
	}

	return 0;
}

void Session::create_nodes()
{
	LOG(INFO) << "creating nodes ... ";

	// creating nodes
	for (int i = 0; i < _block->block_param()->node_size(); ++i)
	{
		auto node_param = _block->block_param()->mutable_node(i);
		auto node = _create_node(node_param);
		//LOG_IF(INFO, dbl > 2) << "creating node " << node->name();
		node->createIO();
		LOG_IF(FATAL, node->inputs().size() != node->param()->input_size()) << "Node " << node->name() << "'s input size " << node->inputs().size() << " does not match the one specified in proto (" << node->param()->input_size() << ")";
		_nodes.push_back(node);
	}

	LOG(INFO) << "connecting nodes ... ";

	// connecting node inputs/outputs
	for (auto node : _nodes) {
		for (int i = 0; i < node->param()->input_size(); ++i) {
			const std::string terminal_name = node->param()->input(i);
			if (!terminal_name.empty()) {
				auto terminal = _find_node_output_by_name(terminal_name);
				LOG_IF(FATAL, terminal == 0) << "Failed to find " << terminal_name << " for node " << node->name();
				node->input(i)->connect(terminal);
			}
		}
	}

	// caching the name of all variables
	_variables = _get_nodes<Variable>("");

	for (auto var : _variables) {
		std::shared_ptr<Solver> solver;
		std::string var_solver_name = var->param()->variable_param().solver_name();
		for (int i = 0; i < _block->block_param()->solver_size(); ++i)
		{
			auto solver_param = _block->block_param()->mutable_solver(i);
			if (var_solver_name == solver_param->name()) {
				solver = _create_solver(solver_param);
				break;
			}
		}
		if (!var_solver_name.empty() && solver == nullptr) {
			LOG(FATAL) << "solver " << var_solver_name << " couldn't be found for variable " << var->name();
		}
		else if (solver) {
			_solvers.insert(std::pair<std::shared_ptr<Variable>, std::shared_ptr<Solver>>(var, solver));
			LOG(INFO) << "variable " << var->name() << " <-> solver " << solver->name();
		}
		else {
			LOG(INFO) << "variable " << var->name() << " <-> constant";
		}
	}

	_insert_splits();

	_created = true;
}

void Session::initialize(std::shared_ptr<ExecutionContext> execution_context) {
	if (_initialized == true)
		return;
	
	if (_created == false)
		create_nodes();

	LOG(INFO) << "initializing ... ";

	int dbl = 0;
	if (execution_context) {
		dbl = execution_context->debug_level;
	}

	_initialized = true;	

	size_t free_byte_before, free_byte_after;
	size_t total_byte;	
	std::srand(std::time(0));
	std::list<std::shared_ptr<Node>> queue = _nodes;
	while (!queue.empty()) {
		auto node = queue.front();
		queue.pop_front();
		if (node->isInitialized())
			continue;
		bool resolved = true;
		for (auto input : node->inputs()) {
			auto connectedNode = input->connectedNode();
			if (connectedNode) {
				if (connectedNode->isInitialized() == false) {
					resolved = false;
					break;
				}
			}
		}
		if (resolved) {
			mem_usage(&free_byte_before, &total_byte, 0);
			node->init();						
			LOG_IF(FATAL, cudaPeekAtLastError() != 0) << "[FAILED] " << node->name() << " | " << cudaGetErrorString(cudaPeekAtLastError());
			mem_usage(&free_byte_after, &total_byte, 0);
			std::string shape;
			int n_outputs = node->outputs().size();
			if (n_outputs > 0) {
				shape = node->output(0)->value()->shape();
				for (int i = 1; i < n_outputs; ++i) {
					shape += "," + node->output(i)->value()->shape();
				}
			}
			float used_memory = (10e-6f * (free_byte_before - free_byte_after)); // MB
			LOG(INFO) << node->op_name() << " " << node->name() << " | " << shape;
			node->setInitialized(true);
		}
		else {
			queue.push_back(node);
		}
	}

	if (execution_context) {
		set_execution_context(execution_context);
	}
	else {
		auto execution_context = std::make_shared<ExecutionContext>();
		set_execution_context(execution_context);
	}	
	
	print_total_parameters("");
}

void Session::_insert_splits()
{	
	for (auto node : _nodes) {
		for (auto output : node->outputs()) {			
			auto connected_terminals = output->connectedTerminals();			
			if (connected_terminals.size() > 1) {

				auto node_param = _block->add_node_param();				
				node_param->set_name("split_" + output->name());
				node_param->add_input(node_param->name());
				for (int i = 0; i < connected_terminals.size(); ++i) {
					node_param->add_output(node_param->name() + "_" + std::to_string(i));
				}
				auto split_param = node_param->mutable_split_param();
				split_param->set_num_outputs(connected_terminals.size());

				auto new_split_node = _create_node(node_param);
				new_split_node->createIO();

				output->disconnect();
				new_split_node->input(0)->connect(output);				

				int i = 0;
				for (auto t : connected_terminals) {
					auto tcast = std::dynamic_pointer_cast<NodeInput>(t);
					if (tcast) {
						tcast->disconnect();
						tcast->connect(new_split_node->output(i++));
					}
				}

				_nodes.push_back(new_split_node);	
			}
		}
	}

	// verification
	for (auto node : _nodes) {
		for (auto output : node->outputs()) {
			auto connected_terminals = output->connectedTerminals();
			LOG_IF(FATAL, connected_terminals.size() > 1);
		}
	}	
}

void Session::set_execution_context(std::shared_ptr<ExecutionContext> execution_context)
{
	_execution_context = execution_context;
	for (auto node : _nodes)
		node->setExecutionContext(execution_context);
}

std::shared_ptr<Node> Session::_find_node_by_name(const std::string &name, const std::string &scope) const {
	if (scope.empty()) {
		for (auto node : _nodes) {
			if (node->name() == name)
				return node;
		}
	}
	else {
		for (auto node : _nodes) {
			if (node->name() == name && node->scope() == scope)
				return node;
		}
	}
	return 0;
}

bool Session::is_end_node(std::shared_ptr<Node> node) const
{
	int num_connected_inputs = 0;
	for (auto input : node->inputs())
		num_connected_inputs += input->connectedNode() ? 1 : 0;
	int num_connected_outputs = 0;
	for (auto output : node->outputs())
		num_connected_outputs += output->connectedNodes().size();
	int num_connected_inputs_by_node = node->inputNodes().size();
	if (num_connected_inputs == 0) {
		return num_connected_outputs == 0;
	}
	else {
		return num_connected_outputs == 0 && num_connected_inputs_by_node != 0;
	}
}

bool Session::is_head_node(std::shared_ptr<Node> node) const
{
	int num_connected_inputs = 0;
	for (auto input : node->inputs())
		num_connected_inputs += input->connectedNode() ? 1 : 0;
	int num_connected_outputs = 0;
	for (auto output : node->outputs())
		num_connected_outputs += output->connectedNodes().size();
	int num_connected_outputs_by_node = node->outputNodes().size();
	if (num_connected_outputs == 0) {
		return num_connected_inputs == 0;
	}
	else {
		return num_connected_inputs == 0 && num_connected_outputs_by_node != 0;
	}
	return false;
}

std::list<std::shared_ptr<Node>> Session::end_nodes(const std::string &scope) const
{
	std::list<std::shared_ptr<Node>> list;
	if (scope.empty()) {
		for (auto node : _nodes) {
			if (is_end_node(node)) {
				list.push_back(node);
			}
		}
	}
	else {
		for (auto node : _nodes) {
			if (node->scope() == scope && is_end_node(node)) {
				list.push_back(node);
			}
		}
	}
	return list;
}

std::shared_ptr<NodeOutput> Session::_find_node_output_by_name(const std::string &name) const {
	for (auto node : _nodes) {
		for (auto terminal : node->outputs()) {
			if (terminal->name() == name)
				return terminal;
		}
	}
	return 0;
}

std::list<std::shared_ptr<Node>> forward_path(std::list<std::shared_ptr<Node>> nodes)
{
	std::list<std::shared_ptr<Node>> active_head_nodes;
	std::list<std::shared_ptr<Node>> visited_nodes;

	auto isVisited = [](std::list<std::shared_ptr<Node>> &list, std::shared_ptr<Node> node_to_check) {
		for (auto node : list)
			if (node == node_to_check)
				return true;
		return false;
	};

	int _verbose = 0;
	while (nodes.size() > 0) {
		auto node = nodes.front();
		_verbose = node->executionContext()->debug_level;
		nodes.pop_front();
		auto inputNodes = node->inputNodes();
		auto outputNodes = node->outputNodes();
		if (isVisited(visited_nodes, node)) {
			continue;
		}
		visited_nodes.push_back(node);
		if (inputNodes.size() > 0) {
			for (auto inputNode : inputNodes) {
				for (auto t : inputNode->outputs()) {
					for (auto c : t->connectedNodes()) {
						if (c == node) {
							t->setEnabled(true);
							break;
						}
					}
				}
				nodes.push_back(inputNode);
			}
		}
		else {
			active_head_nodes.push_back(node);
		}

	}


	nodes = active_head_nodes;
	std::list<std::shared_ptr<Node>> forward_path;
	visited_nodes.clear();

	while (nodes.size() > 0) {
		auto node = nodes.front();
		_verbose = node->executionContext()->debug_level;
		LOG_IF(INFO, _verbose > 3) << "FWRD POP " << node->name();
		nodes.pop_front();
		auto inputNodes = node->inputNodes();
		auto outputNodes = node->outputNodes();
		if (isVisited(visited_nodes, node)) {
			LOG_IF(INFO, _verbose > 3) << "END FOR " << node->name() << " | ALREADY VISITED.";
			continue;
		}
		if (inputNodes.size() == 0 && outputNodes.size() == 0) {
			visited_nodes.push_back(node);
			forward_path.push_front(node);
			LOG_IF(INFO, _verbose > 3) << "END FOR " << node->name() << "  | NO INPUT & OUTPUT.";
			continue;
		}
		if (inputNodes.size() > 0) {
			bool ready = true;
			for (auto inputNode : inputNodes) {
				if (!isVisited(visited_nodes, inputNode))
				{
					LOG_IF(INFO, _verbose > 3) << "REPUSH " << node->name() << " | " << inputNode->name();
					ready = false;
					break;
				}
			}
			if (!ready) {
				nodes.push_back(node);
				continue;
			}
		}
		visited_nodes.push_back(node);		
		forward_path.push_back(node);
		if (outputNodes.size() > 0) {
			for (auto outputT : node->outputs()) {
				if (outputT->enabled()) {
					for (auto outputNode : outputT->connectedNodes()) {
						LOG_IF(INFO, _verbose > 3) << "PUSH " << outputNode->name();
						nodes.push_back(outputNode);
					}
				}
			}
		}
	}
	return forward_path;	
}

void Session::forward(std::list<std::shared_ptr<Node>> end_nodes, std::list<std::pair<std::shared_ptr<PlaceHolder>, std::shared_ptr<Tensor>>> feed_list)
{
	for (auto node : _nodes) {
		for (auto t : node->outputs())
			t->setEnabled(false);
	}
	for (auto pair : feed_list) {
		pair.first->write_values(pair.second);
	}
	auto path = forward_path(end_nodes);
	while (path.size() > 0) {
		auto node = path.front();
		path.pop_front();
		int _verbose = node->executionContext()->debug_level;
		LOG_IF(INFO, _verbose > 2) << "FWRD -> " << node->name();
		node->_forward();
		LOG_IF(FATAL, cudaPeekAtLastError() != 0) << "[FAILED] " << node->name() << " | " << cudaGetErrorString(cudaPeekAtLastError());
	}
}

void Session::forward(const std::string & scope, std::list<std::pair<std::shared_ptr<PlaceHolder>, std::shared_ptr<Tensor>>> feed_list)
{
	forward(end_nodes(scope), feed_list);
}

void Session::reset_gradients(const std::string &scope)
{
	auto variables = _get_nodes<Variable>(scope);	
	for (auto var : variables) {
		var->reset_gradients();
	}
}

void Session::clamp(float min, float max, const std::string &scope)
{
	auto variables = _get_nodes<Variable>(scope);
	for (auto var : variables) {		
		var->clamp(min, max);
	}
}

void Session::backward(std::list<std::shared_ptr<Node>> end_nodes, std::list <std::pair<std::shared_ptr<Node>, std::shared_ptr<Tensor>>> feed_list)
{
	for (auto node : _nodes) {
		for (auto t : node->outputs())
			t->setEnabled(false);
	}
	for (auto pair : feed_list) {
		pair.first->write_diffs(pair.second);
	}
	auto path = forward_path(end_nodes);
	while (path.size() > 0) {
		auto node = path.back();
		path.pop_back();
		int _verbose = node->executionContext()->debug_level;
		LOG_IF(INFO, _verbose > 2) << "BWRD -> " << node->name();
		node->_backward();
		LOG_IF(FATAL, cudaPeekAtLastError() != 0) << "[FAILED] " << node->name() << " | " << cudaGetErrorString(cudaPeekAtLastError());
	}
}

void Session::backward(const std::string & scope, std::list<std::pair<std::shared_ptr<Node>, std::shared_ptr<Tensor>>> feed_list)
{
	backward(end_nodes(scope), feed_list);
}

void Session::apply_solvers(std::list<std::string> solver_names)
{
	if (solver_names.size() > 0) {		
		for (auto item : _solvers) {
			for (auto name : solver_names) {
				if (item.second->name() == name) {										
					item.second->apply(item.first);
				}
			}
		}
	}
	else {
		for (auto item : _solvers) {
			item.second->apply(item.first);
		}
	}
}

void Session::apply_solvers(const std::string & scope)
{	
	for (auto item : _solvers) {
		if (!scope.empty() && item.first->scope() != scope) {			
			continue;
		}
		item.second->apply(item.first);
	}
}

void Session::set_enabled_solvers(bool state, std::list<std::string> solver_names)
{
	if (solver_names.size() > 0) {
		for (auto item : _solvers) {
			for (auto name : solver_names) {
				if (item.second->name() == name) {
					item.second->set_enabled(state);
				}
			}
		}
	}
	else {
		for (auto item : _solvers)
			item.second->set_enabled(state);			
	}
}

void Session::set_enabled_solvers(bool state, const std::string & scope)
{
	if (scope.empty()) {
		for (auto item : _solvers)
			item.second->set_enabled(state);
	}
	else {
		for (auto item : _solvers) {
			if (item.first->scope() == scope)
				item.second->set_enabled(state);
		}
	}
}

void Session::set_learning_rate(float lr, std::list<std::string> solver_names)
{
	if (solver_names.size() > 0) {
		for (auto item : _solvers) {
			for (auto name : solver_names) {
				if (item.second->name() == name) {
					item.second->set_learning_rate(lr);
				}
			}
		}
	}
	else {
		for (auto item : _solvers)
			item.second->set_learning_rate(lr);			
	}
}

void Session::set_learning_rate(float lr, const std::string & scope)
{
	if (scope.empty()) {
		for (auto item : _solvers) {
				item.second->set_learning_rate(lr);
		}
	}
	else {
		for (auto item : _solvers) {
			if (item.first->scope() == scope) {
				item.second->set_learning_rate(lr);
			}
		}
	}
}

void Session::save(std::string file_path, bool as_text)
{
	for (auto node : _nodes) {
		node->prep_for_saving();
	}
	if (as_text)
		_block->save_as_text(file_path);
	else
		_block->save_as_binary(file_path);
}

void Session::print_variables_info(const std::string &scope)
{
	std::list<std::shared_ptr<Variable>> variable_nodes = _get_nodes<Variable>(scope);	
	double mean, std, min, max;
	for (auto var : variable_nodes) {
		auto output = var->output(0);
		output->value()->statistics(&mean, &std, &min, &max);
		LOG(INFO) << output->value()->name() << " | " << mean << " " << std << " | " << min << " " << max;
		if (output->diff()) {
			output->diff()->statistics(&mean, &std, &min, &max);
			LOG(INFO) << output->diff()->name() << " | " << mean << " " << std << " | " << min << " " << max;
		}		
	}
}

void Session::print_nodes_info(const std::string &scope)
{	
	double mean, std, min, max;
	for (auto node : _nodes) {
		if (!scope.empty() && node->scope() != scope) continue;
		for (auto output : node->outputs()) {
			output->value()->statistics(&mean, &std, &min, &max);
			LOG(INFO) << output->value()->name() << " | "<< mean << " " << std << " | " << min << " " << max;
			if (output->diff()) {
				output->diff()->statistics(&mean, &std, &min, &max);
				LOG(INFO) << output->diff()->name() << " | " << mean << " " << std << " | " << min << " " << max;
			}
			output->value()->gpu_data();
		}
	}	
}

void Session::print_nodes(const std::string & scope)
{
	for (auto node : _nodes) {
		if (!scope.empty() && node->scope() != scope) continue;
		node->print();
	}
}

std::shared_ptr<Node> Session::end_node(const std::string &scope) const
{
	auto list = end_nodes(scope);	
	LOG_IF(FATAL, list.size() != 1) << "[FAILED] session must have exactly one end node.";
	return *list.begin();
}

void Session::print_total_parameters(const std::string &scope)
{
	std::list<std::shared_ptr<Variable>> variable_nodes = _get_nodes<Variable>(scope);
	size_t total = 0;
	for (auto var : variable_nodes) {
		total += var->output(0)->value()->size();
	}
	LOG(INFO) << "total parameters: " << total;
}

void Session::mem_usage(size_t * free_byte, size_t * total_byte, float *used_byte_percentage)
{
	cudaError_t cuda_status = cudaMemGetInfo(free_byte, total_byte);
	if (cudaSuccess != cuda_status)
		LOG(FATAL) << "cudaMemGetInfo fails, " << cudaGetErrorString(cuda_status);
	if (used_byte_percentage)
		*used_byte_percentage = ((float)*total_byte - (float)*free_byte) / (*total_byte) * 100;
}

void generate_cpp_code(Node *node, std::string *code) {	
	(*code) += node->to_cpp() + "\n";	
}

std::string Session::to_cpp(const std::string &scope) const
{
	std::string code = "\nDeepFlow df;\n\n";

	std::set<std::shared_ptr<Solver>> solvers_set;
	for (auto solver_map_item : _solvers) {
		bool exist = false;
		for (auto solver : solvers_set) {
			if (solver->name() == solver_map_item.second->name())
			{
				exist = true;
				break;
			}
		}
		if (!exist)
			solvers_set.insert(solver_map_item.second);
	}
	for (auto solver : solvers_set)
		code += solver->to_cpp() + "\n";

	if (solvers_set.size() > 0)
		code += "\n";

	std::function<void(Node*)> foo = std::bind(generate_cpp_code, std::placeholders::_1, &code);

	int token = rand();

	std::list<std::shared_ptr<Node>> nodes = end_nodes(scope);
	std::list<std::shared_ptr<Node>> list, visited_nodes;

	auto isVisited = [](std::list<std::shared_ptr<Node>> &list, std::shared_ptr<Node> node_to_check) {
		for (auto node : list)
			if (node == node_to_check)
				return true;
		return false;
	};

	while (nodes.size() > 0) {
		auto node = nodes.front();
		nodes.pop_front();
		auto inputNodes = node->inputNodes();
		auto outputNodes = node->outputNodes();
		if (isVisited(visited_nodes, node)) {
			continue;
		}
		visited_nodes.push_back(node);
		if (inputNodes.size() == 0 && outputNodes.size() == 0) {
			continue;
		}
		if (inputNodes.size() > 0) {
			for (auto inputNode : inputNodes) {
				for (auto t : inputNode->outputs()) {
					for (auto c : t->connectedNodes()) {
						if (c == node) {
							t->setEnabled(true);
							break;
						}
					}
				}
				nodes.push_front(inputNode);
			}
		}

		list.push_front(node);
	}

	for (auto node : list) {
		auto cpp = node->to_cpp();
		code += (cpp.empty()?"MISSING " + node->name() : cpp) + "\n";
	}
	code += "\n";

	return code;
}

std::shared_ptr<PlaceHolder> Session::get_placeholder(const std::string &name, const std::string &scope)
{
	auto node = _find_node_by_name(name, scope);
	LOG_IF(FATAL, node == nullptr) << "Node " << name << " does not exist.";
	auto placeholder = std::dynamic_pointer_cast<PlaceHolder>(node);
	LOG_IF(FATAL, placeholder == nullptr) << name << " is not a placeholder.";
	return placeholder;
}