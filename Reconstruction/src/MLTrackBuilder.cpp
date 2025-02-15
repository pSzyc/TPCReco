#include "TPCReco/MLTrackBuilder.h"
#include <iostream>
#include "TPCReco/tf_functions.h"
#include <boost/property_tree/json_parser.hpp>

TensorflowModel::TensorflowModel(const boost::property_tree::ptree& aConfig)
    : graph(nullptr), session(nullptr), myConfig(aConfig), output_lenght(1)
{
    const std::string model_path = myConfig.get<std::string>("input.TfModelPath");
    const char* model_path_cstr = model_path.c_str();

    // Parse the input dimensions
    for (const auto& item : myConfig.get_child("input.InputDim")) {
        inputDim.push_back(item.second.get_value<std::int64_t>());
    }
    // Parse the output dimensions
    for (const auto& item : myConfig.get_child("input.OutputDim")) {
        outputDim.push_back(item.second.get_value<std::int64_t>());
        output_lenght *= item.second.get_value<std::int64_t>();
    }

    // Load the TensorFlow model session.
    tf_functions::load_session(model_path_cstr, &graph, &session);

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

std::vector<float> TensorflowModel::run(std::vector<float> input_data)
{    

    // Create the input tensor using the stored dimensions.
    TF_Tensor* input_tensor = nullptr;
    tf_functions::create_tensor(TF_FLOAT, inputDim, inputDim.size(), input_data, &input_tensor);


    // Run the session.
    TF_Tensor* output_tensor = nullptr;
    tf_functions::run_session(
        session,
        &input, &input_tensor, 1,
        &output, &output_tensor, 1
    );
    // Retrieve the results from the output tensor.
    float* tensor_data = static_cast<float*>(TF_TensorData(output_tensor));

    std::vector<float> results;

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