#ifndef TENSORFLOWMODEL_H
#define TENSORFLOWMODEL_H

#include <vector>
#include <cstdint>
#include <tensorflow/c/c_api.h>
#include <boost/property_tree/ptree.hpp>

class TensorflowModel {
public:

    TensorflowModel(const boost::property_tree::ptree& aConfig);

    std::vector<float> run(std::vector<float>  input_data);

    ~TensorflowModel();

private:
    TF_Graph* graph;
    TF_Session* session;
    TF_Output input;
    TF_Output output;
	boost::property_tree::ptree myConfig;
    std::vector<std::int64_t> inputDim = {};
    std::vector<std::int64_t> outputDim = {};
    std::int64_t output_lenght;
};

#endif // TENSORFLOWMODEL_H