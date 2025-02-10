#ifndef TENSORFLOWMODEL_H
#define TENSORFLOWMODEL_H

#include <vector>
#include <cstdint>
#include <tensorflow/c/c_api.h>

// Structure to hold the output tensor data together with its shape.
struct Tensor {
    std::vector<std::int64_t> shape;
    std::vector<float> data;
};

class TensorflowModel {
public:

TensorflowModel(const char* model_path);

    Tensor run(const Tensor input_data, std::vector<int64_t> output_shape);

    ~TensorflowModel();

private:
    TF_Graph* graph;
    TF_Session* session;
    TF_Output input;
    TF_Output output;
};

#endif // TENSORFLOWMODEL_H