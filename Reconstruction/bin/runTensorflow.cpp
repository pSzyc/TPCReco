#include <iostream>
#include <numeric>  // for accumulate
#include <boost/property_tree/json_parser.hpp>

#include "TPCReco/MLTrackBuilder.h"
#include "TPCReco/ConfigManager.h"
#include "TPCReco/RunController.h"
#include "TPCReco/EventTPC.h"

// Necessary for Monte Carlo module to generate data
#include "../../MonteCarlo/Modules/DummyModule/DummyModule.h"

int main(int argc, char** argv) {
    ConfigManager cm;
    boost::property_tree::ptree tree = cm.getConfig(argc, argv);
    if (cm.isHelpMode()) return 0;

    // Read config values
    std::string controllerConfigPath = tree.get<std::string>("input.controllerConfigPath");
    std::string geometryFileName     = tree.get<std::string>("input.geometryFile");

    boost::property_tree::ptree controllerConfig;
    boost::property_tree::read_json(controllerConfigPath, controllerConfig);

    // Load geometry
    auto myGeometryPtr = std::make_shared<GeometryTPC>(geometryFileName.c_str(), false);
    if (!myGeometryPtr) {
        std::cerr << "Error: Geometry not loaded!" << std::endl;
        return 1;
    }

    // Initialize run controller for MC data
    auto runController = std::make_shared<fwk::RunController>();
    runController->Init(controllerConfig);

    // Load TensorFlow model
    TensorflowModel model(tree);

    while (true) {
        runController->RunSingle();
        PEventTPC pEventTPC = runController->getCurrentPEventTPC();

        //handling invalid peventtpc
        // if (!pEventTPC) {
        //     std::cerr << "No valid event received. Skipping..." << std::endl;
        //     continue;
        // }

        // Convert PEventTPC to EventTPC
        EventTPC eventTPC;
        eventTPC.SetGeoPtr(myGeometryPtr);
        eventTPC.Clear();
        eventTPC.SetChargeMap(pEventTPC.GetChargeMap());
        eventTPC.SetEventInfo(pEventTPC.GetEventInfo());

        // Flatten histogram into input tensor
        auto histPtr = eventTPC.GetRawHisto();
        std::vector<float> input_tensor;
        constexpr size_t MODEL_INPUT_SIZE = 256 * 512 * 3;

        if (!histPtr) {
            std::cerr << "Warning: No histogram found. Using zero input." << std::endl;
            input_tensor.resize(MODEL_INPUT_SIZE, 0.0f);
        } else {
            int nBinsX = histPtr->GetNbinsX();
            int nBinsY = histPtr->GetNbinsY();
            int nBinsZ = histPtr->GetNbinsZ();

            for (int x = 1; x <= nBinsX; ++x)
                for (int y = 1; y <= nBinsY; ++y)
                    for (int z = 1; z <= nBinsZ; ++z)
                        input_tensor.push_back(histPtr->GetBinContent(x, y, z));

            if (input_tensor.size() < MODEL_INPUT_SIZE)
                input_tensor.resize(MODEL_INPUT_SIZE, 0.0f);
            else if (input_tensor.size() > MODEL_INPUT_SIZE)
                input_tensor.resize(MODEL_INPUT_SIZE);
        }

        //the input tensor turns out to be all zeros
        std::cout << "\n[DEBUG] Input tensor (first 100 values):\n";
        for (size_t i = 0; i < std::min(input_tensor.size(), size_t(100)); ++i) {
            std::cout << input_tensor[i] << " ";
            if ((i + 1) % 10 == 0) std::cout << std::endl;
        }
        std::cout << std::endl;

        float sum = std::accumulate(input_tensor.begin(), input_tensor.end(), 0.0f);
        float max = *std::max_element(input_tensor.begin(), input_tensor.end());
        std::cout << "[DEBUG] Input tensor sum: " << sum << ", max value: " << max << "\n" << std::endl;

        // Run inference
        std::vector<float> output_tensor = model.run(input_tensor);

        // Display results
        std::cout << "Prediction:" << std::endl;
        for (float val : output_tensor)
            std::cout << val << std::endl;

        std::cout << "Press Enter to continue or type 'exit' to break the loop: ";
        std::string user_input;
        std::getline(std::cin, user_input);
        if (user_input == "exit") break;
    }

    return 0;
}
