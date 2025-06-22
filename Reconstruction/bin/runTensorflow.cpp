#include <iostream>
#include <numeric>  // for accumulate
#include <boost/property_tree/json_parser.hpp>
#include <tuple>

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

        auto u = eventTPC.get2DProjection(definitions::projection_type::DIR_TIME_U, filter_type::none, scale_type::mm);
        auto v = eventTPC.get2DProjection(definitions::projection_type::DIR_TIME_V, filter_type::none, scale_type::mm);
        auto w = eventTPC.get2DProjection(definitions::projection_type::DIR_TIME_W, filter_type::none, scale_type::mm);

        assert(u->GetNbinsZ() == 1);
        assert(v->GetNbinsZ() == 1);
        assert(w->GetNbinsZ() == 1);

        auto bufs =
            std::array<std::tuple<const TH2D*, std::size_t, std::size_t>, 3>{
                {std::make_tuple(u.get(), u->GetNbinsX(), u->GetNbinsY()),
                 std::make_tuple(v.get(), v->GetNbinsX(), v->GetNbinsY()),
                 std::make_tuple(w.get(), w->GetNbinsX(), w->GetNbinsY())}};


        constexpr std::size_t LAYER_X_SIZE = 512;
        constexpr std::size_t LAYER_Y_SIZE = 256;
        auto model_input_tensor = std::vector<float>(LAYER_X_SIZE * LAYER_Y_SIZE * 3);

        auto idx = 0u;
        for (auto const& buf_tup : bufs) {
          auto data = std::get<0>(buf_tup);
          auto len_x = std::get<1>(buf_tup);
          auto len_y = std::get<2>(buf_tup);

          assert(len_x <= LAYER_X_SIZE);
          assert(len_y <= LAYER_Y_SIZE);

          //memory layout: [x=0,y=0, x=0, y=1, ..., x=0, y=n, x=1, y=n, ...]
          for(auto x = 0u; x < len_x; ++x){
            auto col_it = model_input_tensor.begin() + (x * LAYER_Y_SIZE) + (idx * LAYER_X_SIZE * LAYER_Y_SIZE);
            for(auto y = 0u; y < len_y; ++y){
              *col_it = data->GetBin(x, y);
              ++col_it;
            }
          }
          ++idx;
        }

        auto output_tensor = model.run(model_input_tensor);

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
