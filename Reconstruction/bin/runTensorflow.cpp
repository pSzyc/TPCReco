#include <iostream>

#include <boost/property_tree/json_parser.hpp>

#include "TPCReco/MLTrackBuilder.h"
#include "TPCReco/ConfigManager.h"

// Function to create a tensor filled with ones for testing
std::vector<float> createVectorOfOnes(int batch_size) {
    const int height = 256;
    const int width = 512;
    const int channels = 3;

    std::vector<float> input_example = {};

    input_example.resize(batch_size * height * width * channels, 1.0f);

    return input_example;
}

int main(int argc, char** argv) {
	ConfigManager cm;
	boost::property_tree::ptree tree = cm.getConfig(argc,argv);
    if(cm.isHelpMode()) return 0; // nothing more to do, exit

    int batch_size = 1;
    std::vector<float> input_tensor = createVectorOfOnes(batch_size);
        TensorflowModel model(tree);

    // Tensor output_tensor = model.run(input_tensor, output_shape);

    // std::cout << "Prediction:" << std::endl;
    // for (size_t i = 0; i < output_tensor.data.size(); ++i) {
    //     std::cout << output_tensor.data[i] << std::endl;
    // }

    return 0;
}