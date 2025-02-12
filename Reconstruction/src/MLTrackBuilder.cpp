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

std::vector<float> TensorflowModel::run(std::vector<float> input_data, size_t batch_size)
{    
    
    std::vector<size_t> input_dim = myConfig.get<std::vector<size_t>>("MLTrackBuilder.InputDim")
    std::vector<size_t> output_dim = myConfig.get<std::vector<size_t>>("MLTrackBuilder.OutputDim")


    input_dim.insert(input_dim.first(), batch_size)
    output_dim.insert(output_dim.first(), batch_size)

    
    TF_Tensor* input_tensor = nullptr;
    tf_functions::create_tensor(TF_FLOAT, input_dim, input_dim.size(), input_data, &input_tensor);


    // Run the session.
    TF_Tensor* output_tensor = nullptr;
    tf_functions::run_session(session,
                              &input, &input_tensor, 1,
                              &output, &output_tensor, 1);

    // Retrieve the results from the output tensor.
    float* tensor_data = static_cast<float*>(TF_TensorData(output_tensor));

    std::vector<float> results;
    int64_t output_lenght = 1;
    for (auto el: output_dim){
        output_lenght *= el;
    }
    results.reserve(output_lenght);

    for (std::int64_t i = 0; i < output_lenght; i++) {
        results.push_back(tensor_data[i]);
    }

    // Clean up the temporary tensors.
    tf_functions::delete_tensor(input_tensor);
    tf_functions::delete_tensor(output_tensor);

    return results;
}

TensorflowModel::~TensorflowModel()
{
    // Release the graph and session.
    tf_functions::delete_graph(graph);
    tf_functions::delete_session(session);
}