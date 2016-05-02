#pragma once
#include <string>

class Configuration
{
public:
    Configuration(const std::string& filepath);
    std::string demo_path;
private:
    // TODO only one demo so far
};
