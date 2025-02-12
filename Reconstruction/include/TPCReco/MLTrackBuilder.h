#ifndef TENSORFLOWMODEL_H
#define TENSORFLOWMODEL_H

#include <vector>
#include <cstdint>
#include <tensorflow/c/c_api.h>

class TensorflowModel {
public:

    TensorflowModel(const char* model_path, const boost::property_tree::ptree& aConfig);

    std::vector<float> data run(std::vector<float> data);
    
    ~TensorflowModel();

private:
    TF_Graph* graph;
    TF_Session* session;
    TF_Output input;
    TF_Output output;
	boost::property_tree::ptree myConfig;
};

#endif // TENSORFLOWMODEL_H