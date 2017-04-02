#pragma once
#include <string>
#include <vector>

struct DemoConfig {
    std::string path;
};

class Configuration
{
public:
    Configuration(const std::string& filepath);

    // Config
    int patience;
    bool authentication;
    std::vector<DemoConfig> demos;
    //
    int login_patience;
    std::string login_path;

private:
    void ReadConfigFile(std::ifstream&);
    void ReadConfigFile_tinytoml(std::ifstream&);

    /* TODO static default config variables */
};
