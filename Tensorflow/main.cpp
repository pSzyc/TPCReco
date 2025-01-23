#include <tensorflow/c/c_api.h>
#include <iostream>
#include "tf_functions.h"

// just for tests, please delete it later
std::vector<std::vector<std::vector<std::vector<int>>>> createVectorOfOnes(int batch_size) {
    const int height = 256;
    const int width = 512;
    const int channels = 3;

    // Create a 4D vector filled with ones
    std::vector<std::vector<std::vector<std::vector<int>>>> vec(
        batch_size, 
        std::vector<std::vector<std::vector<int>>>(
            height, 
            std::vector<std::vector<int>>(
                width, 
                std::vector<int>(channels, 1)
            )
        )
    );

    return vec;
}

int main () {
    int batch_size = 1; // Example batch size
    auto x_test = createVectorOfOnes(batch_size);

    char model_path[] = "../model-test";
    TF_Graph * graph = nullptr;
    TF_Session * session = nullptr;
    TF_Tensor * input_tensor = nullptr, * output_tensor = nullptr;

    tf_functions::load_session(model_path, &graph, &session);

    TF_Operation * input_op = TF_GraphOperationByName(graph, "serve_keras_tensor");
    TF_Output input = TF_Output{input_op, 0};
    if (input.oper == nullptr) {
        std::cout << "Can't init input_op" << std::endl;
        return 2;
    }

    std::cout << "init input_op" << std::endl;

    TF_Operation * output_op = TF_GraphOperationByName(graph, "StatefulPartitionedCall");
    TF_Output output = TF_Output{output_op, 0};
    if (output.oper == nullptr) {
        std::cout << "Can't init output_op" << std::endl;
        return 2;
    }

    std::cout << "init output_op" << std::endl;

    std::vector<std::int64_t> dims = {batch_size, 256, 512, 3};
    int num_dims = 4;
    tf_functions::create_tensor(TF_FLOAT, dims, num_dims, x_test, &input_tensor);

    tf_functions::run_session(session,
        &input, &input_tensor, 1,
        &output, &output_tensor, 1);

    // Check results
    auto tensor_data = static_cast<float*>(TF_TensorData(output_tensor));

    std::cout << std::endl << "Prediction:" << std::endl;

    for (int i = 0; i < 9; i++) {
        std::cout << tensor_data[i] << std::endl;
    }
    tf_functions::delete_tensor(input_tensor);
    tf_functions::delete_tensor(output_tensor);
    tf_functions::delete_graph(graph);
    tf_functions::delete_session(session);

    return 0;
}