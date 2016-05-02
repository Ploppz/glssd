#include "Configuration.h"
#include <fstream>
#include <iostream>

Configuration::Configuration(const std::string& filepath )
{
    std::ifstream file(filepath);
    if (file.is_open()) {
        getline(file, demo_path);
    } else {
        std::cout << "Unable to open config file " << filepath << " for reading. " << std::endl;
    }
    file.close();
}
