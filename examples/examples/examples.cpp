
#include "core/deep_flow.h"
#include "core/session.h"

#include <gflags/gflags.h>

DEFINE_string(mnist, "./data/mnist", "Path to MNIST data folder");
DEFINE_string(i, "", "Trained network model to load");
DEFINE_string(o, "", "Trained network model to save");
DEFINE_bool(text, false, "Save model as text");
DEFINE_bool(includeweights, false, "Also save weights in text mode");
DEFINE_bool(includeinits, false, "Also save initial values");
DEFINE_int32(batch, 100, "Batch size");
DEFINE_string(run,"", "Phase to execute graph");
DEFINE_bool(printiter, false, "Print iteration message");
DEFINE_bool(printepoch, true, "Print epoch message");
DEFINE_int32(debug, 0, "Level of debug");
DEFINE_int32(epoch, 1000, "Maximum epochs");
DEFINE_int32(iter, -1, "Maximum iterations");
DEFINE_string(image, "lena-256x256.jpg", "Input image");
DEFINE_int32(examp1, 0, "Eucliean image reconstruction");
DEFINE_int32(examp2, 0, "Transposed convolution image reconstruction test");

void main(int argc, char** argv) {
	gflags::ParseCommandLineFlags(&argc, &argv, true);

	CudaHelper::setOptimalThreadsPerBlock();	

	int batch_size = FLAGS_batch;
	
	DeepFlow df;
		
	if (FLAGS_i.empty()) {
		if (FLAGS_examp1 == 1) {
			df.define_phase("Train", PhaseParam_PhaseBehaviour_TRAIN);
			auto solver = df.gain_solver(1.0f, 0.01f, 100, 0.000000001f, 0.05f, 0.95f);
			//auto solver = df.sgd_solver(1.0f, 0.01f);
			//auto solver = df.adam_solver();
			//auto solver = df.adadelta_solver();
			auto image = df.imread(FLAGS_image, ImageReaderParam_Type_COLOR_IF_AVAILABLE);
			auto generator = df.data_generator(df.random_uniform({ 1, 3, 256, 256 }, -1, 1), 1, solver, "gen");
			df.euclidean_loss(generator, image);
			df.display(image, 2, DisplayParam_DisplayType_VALUES, "input", { "Train" });
			df.display(generator, 2, DisplayParam_DisplayType_VALUES, "approximation", { "Train" });
			df.psnr(image, generator, Psnr::EVERY_PASS);
		}
		else if (FLAGS_examp2 == 1) {
			df.define_phase("Train", PhaseParam_PhaseBehaviour_TRAIN);
			auto solver1 = df.gain_solver(0.999f, 0.0001f, 100, 0.000000001f, 0.05f, 0.95f);
			auto solver2 = df.sgd_solver(1.0f, 0.0000000001f);
			//auto solver = df.adam_solver();
			//auto solver = df.adadelta_solver();		
			auto image = df.imread(FLAGS_image, ImageReaderParam_Type_GRAY_ONLY);
			auto stat = df.variable(df.random_uniform({ 1,1,252,252 }, 0.8, 1), solver1);
			auto f = df.variable(df.random_uniform({ 1,1,5,5 }, 0.9, 0.1), solver2);
			auto tconv = df.transposed_conv2d(stat, f, { 1,1,256,256 }, 0, 0, 1, 1, 1, 1);
			df.euclidean_loss(tconv, image);
			df.display(tconv, 20, DisplayParam_DisplayType_VALUES, "input", { "Train" });
			df.psnr(tconv, image, Psnr::EVERY_PASS, "psnr", { "Train" });
		}
	}
	else {
		df.load_from_binary(FLAGS_i);	
	}

	auto session = df.session();	

	if (!FLAGS_run.empty()) {
		session->initialize();
		session->run(FLAGS_run, FLAGS_epoch, FLAGS_iter, FLAGS_printiter, FLAGS_printepoch, FLAGS_debug);
	}

	
	if (!FLAGS_o.empty())
	{
		if (FLAGS_text)
			df.save_as_text(FLAGS_o);
		else
			df.save_as_binary(FLAGS_o);
	}
	
}


