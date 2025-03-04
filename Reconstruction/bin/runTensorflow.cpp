#include <iostream>

#include <boost/property_tree/json_parser.hpp>

#include "TPCReco/MLTrackBuilder.h"
#include "TPCReco/ConfigManager.h"
#include "TPCReco/RunController.h"
#include "TPCReco/EventTPC.h"
// Following line neccesery for using runController
#include "../../MonteCarlo/Modules/DummyModule/DummyModule.h"

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
	boost::property_tree::ptree tree = cm.getConfig(argc, argv);

    if(cm.isHelpMode()) return 0;


    // Config parsing
    std::string controllerConfigPath = tree.get<std::string>("input.controllerConfigPath");
    std::string geometryFileName = tree.get<std::string>("input.geometryFile");

    boost::property_tree::ptree controllerConfig;
    boost::property_tree::read_json(controllerConfigPath, controllerConfig);


    // Geometry
    std::shared_ptr<GeometryTPC> myGeometryPtr;
    myGeometryPtr = std::make_shared<GeometryTPC>(geometryFileName.c_str(), false);
    if(!myGeometryPtr){
        std::cerr<<"Geometry not loaded!"<<std::endl;
        exit(1);
    }

    // Run controller
    auto runController = std::make_shared<fwk::RunController>();
    runController -> Init(controllerConfig);

    // Example initalization
    EventTPC eventTPC = EventTPC();
    eventTPC.SetGeoPtr(myGeometryPtr);

    int batch_size = 1;
    TensorflowModel model(tree);
    std::vector<float> input_tensor; 
    std::vector<float> output_tensor;
    while (true) {
        // Get rid of this dummy example
        input_tensor = createVectorOfOnes(batch_size);


        // Make the model run reconstruction on data from eventTPC.
        
        //runController -> RunSingle();
        //PEventTPC pEventTPC = runController -> getCurrentPEventTPC();
        //eventTPC.Clear();
        //eventTPC.SetChargeMap(pEventTPC.GetChargeMap());
        //eventTPC.SetEventInfo(pEventTPC.GetEventInfo());
    
        output_tensor = model.run(input_tensor);

        std::cout << "Prediction:" << std::endl;
        for (size_t i = 0; i < output_tensor.size(); ++i) {
             std::cout << output_tensor[i] << std::endl;
        }
        std::cout << "Press Enter to continue or type 'exit' to break the loop: ";
        std::string user_input;
        std::getline(std::cin, user_input);
        if (user_input == "exit") {
            break;
        }
    }
    return 0;
}