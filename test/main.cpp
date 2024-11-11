#include <cppflow/ops.h>
#include <cppflow/model.h>

// C++ headers
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <model_path>" << std::endl;
        return 1;
    }

    std::string model_path = argv[1];
    auto input = cppflow::fill({10, 5}, 1.0f);
    cppflow::model model(model_path);
    auto output = model(input);

    std::cout << output << std::endl;

    auto values = output.get_data<float>();

    for (auto v : values) {
        std::cout << v << std::endl;
    }
    return 0;
}