#include "TPCReco/MLTrackBuilder.h"
#include <iostream>
#include "TPCReco/tf_functions.h"
#include <boost/property_tree/json_parser.hpp>

TensorflowModel::TensorflowModel(const char* model_path, const boost::property_tree::ptree& aConfig);
    : graph(nullptr), session(nullptr)
{
    // Load the TensorFlow model session.
    tf_functions::load_session(model_path, &graph, &session);

    // Initialize the input operation.
    TF_Operation* input_op = TF_GraphOperationByName(graph, "serve_keras_tensor");
    input = TF_Output{input_op, 0};
    if (input.oper == nullptr) {
        std::cerr << "Can't init input_op" << std::endl;
    } else {
        std::cout << "Initialized input_op" << std::endl;
    }

    // Initialize the output operation.
    TF_Operation* output_op = TF_GraphOperationByName(graph, "StatefulPartitionedCall");
    output = TF_Output{output_op, 0};
    if (output.oper == nullptr) {
        std::cerr << "Can't init output_op" << std::endl;
    } else {
        std::cout << "Initialized output_op" << std::endl;
    }
}

Tensor TensorflowModel::run(const Tensor input_data, std::vector<int64_t> output_shape)
{    

    // Create the input tensor using the stored dimensions.
    TF_Tensor* input_tensor = nullptr;
    tf_functions::create_tensor(TF_FLOAT, input_data.shape, input_data.shape.size(), input_data.data, &input_tensor);


    // Run the session.
    TF_Tensor* output_tensor = nullptr;
    tf_functions::run_session(session,
                              &input, &input_tensor, 1,
                              &output, &output_tensor, 1);

    // Retrieve the results from the output tensor.
    float* tensor_data = static_cast<float*>(TF_TensorData(output_tensor));

    std::vector<float> results;
    int64_t output_lenght = 1;
    for (auto el: output_shape){
        output_lenght *= el;
    }
    results.reserve(output_lenght);

    for (std::int64_t i = 0; i < output_lenght; i++) {
        results.push_back(tensor_data[i]);
    }

    // Clean up the temporary tensors.
    tf_functions::delete_tensor(input_tensor);
    tf_functions::delete_tensor(output_tensor);

    Tensor tensor = {output_shape, results};
    return tensor;
}

TensorflowModel::~TensorflowModel()
{
    // Release the graph and session.
    tf_functions::delete_graph(graph);
    tf_functions::delete_session(session);
}