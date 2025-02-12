#include <iostream>
#include "TPCReco/MLTrackBuilder.h"
#include "TPCReco/ConfigManager.h"

// Function to create a tensor filled with ones for testing
std::vector<float> createTensorOfOnes(int batch_size) {
    const int height = 256;
    const int width = 512;
    const int channels = 3;

    data = vector of size {batch_size * height * width * channels} with ones 1.0f;

    return data;
}

int main(int argc, char** argv) {

	ConfigManager cm;
	boost::property_tree::ptree tree = cm.getConfig(argc, argv);
    int batch_size = 1; // Example batch size
    std::vector<float> input_tensor = createTensorOfOnes(batch_size);

    const char* model_path = "resources/model";
    TensorflowModel model(model_path, cm);

    std::vector<float> output_tensor = model.run(input_tensor);

    std::cout << "Prediction:" << std::endl;
    for (size_t i = 0; i < output_tensor.size(); ++i) {
        std::cout << output_tensor[i] << std::endl;
    }

    return 0;
}