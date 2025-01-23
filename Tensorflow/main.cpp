#include <tensorflow/c/c_api.h>
#include <iostream>
#include "tf_functions.h"

int main () {

	std::vector<float> x_test_0 = {1, 2, 3, 4, 5};
	std::vector<float> y_test_0 = {1};
    
	char model_path[] = "../model";

    //moze do konstruktora
	TF_Graph * graph = nullptr;
	TF_Session * session = nullptr;
	TF_Tensor * input_tensor = nullptr, * output_tensor = nullptr;

	tf_functions::load_session(model_path, &graph, &session);

	TF_Operation * input_op = TF_GraphOperationByName(graph, "serving_default_input_1");
	TF_Output input = TF_Output{input_op, 0};
	if (input.oper == nullptr) {
		std::cout << "Can't init input_op" << std::endl;
		return 2;
	}

    //info na ekran (useless)
	std::cout << "init input_op" << std::endl;

	TF_Operation * output_op = TF_GraphOperationByName(graph, "StatefulPartitionedCall");
	TF_Output output = TF_Output{output_op, 0};
	if (output.oper == nullptr) {
		std::cout << "Can't init output_op" << std::endl;
		return 2;
	}

	std::cout << "init output_op" << std::endl;

    //przeładowany operator klasy / metoda?
	std::vector<std::int64_t> dims = {1, 5};
	int num_dims = 2;
	tf_functions::create_tensor(TF_FLOAT, dims, num_dims, x_test_0, &input_tensor);

	tf_functions::run_session (session,
		&input, &input_tensor, 1,
		&output, &output_tensor, 1);


	//niech zwraca <vector>
	auto tensor_data = static_cast<float*>(TF_TensorData(output_tensor));

	std::cout << std::endl << "Prediction:" << std::endl;

	for (int i = 0; i < 1; i++) {
        std::cout << tensor_data[i] << std::endl;
    }

    //destruktor
	tf_functions::delete_tensor(input_tensor);
	tf_functions::delete_tensor(output_tensor);
	tf_functions::delete_graph(graph);
	tf_functions::delete_session(session);

	return 0;
}