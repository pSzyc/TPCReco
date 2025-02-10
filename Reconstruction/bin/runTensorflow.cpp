#include <iostream>
#include "TPCReco/MLTrackBuilder.h"

// Function to create a tensor filled with ones for testing
Tensor createTensorOfOnes(int batch_size) {
    const int height = 256;
    const int width = 512;
    const int channels = 3;

    Tensor tensor;
    tensor.shape = {batch_size, height, width, channels};
    tensor.data.resize(batch_size * height * width * channels, 1.0f);

    return tensor;
}

int main() {
    int batch_size = 1; // Example batch size
    Tensor input_tensor = createTensorOfOnes(batch_size);
    std::vector<int64_t> output_shape = {batch_size, 9};

    const char* model_path = "resources/model-test";
    TensorflowModel model(model_path);

    Tensor output_tensor = model.run(input_tensor, output_shape);

    std::cout << "Prediction:" << std::endl;
    for (size_t i = 0; i < output_tensor.data.size(); ++i) {
        std::cout << output_tensor.data[i] << std::endl;
    }

    return 0;
}