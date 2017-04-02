#include "Configuration.h"
#include <fstream>
#include <iostream>
#include <toml/toml.h>
#include <cpptoml.h>

Configuration::Configuration(const std::string& filepath )
{
    std::ifstream file(filepath);
    assert( file.is_open() );
    ReadConfigFile(file);
    file.close();
}
// TODO
// try cpptoml instead: default values and no type checking
void Configuration::ReadConfigFile_tinytoml(std::ifstream& ifs)
{
    toml::ParseResult pr = toml::parse(ifs);
    assert (pr.valid());

    const toml::Value& v = pr.value;

    const toml::Value* x;
    /** Patience **/
    x = v.find("general.patience");
    patience = x->as<int>();

    /** Authentication **/
    x = v.find("general.authentication");
    authentication = x->as<bool>();

    /** Demos table array **/
    const toml::Value* demo_array_val = v.find("demos");
    if (demo_array_val) {
        if (demo_array_val->is<toml::Array>()) {
            const toml::Array demos_array = v.get<toml::Array>("demos");
            for (auto it = demos_array.begin(); it != demos_array.end(); it ++)
            {
                const toml::Value* demo_path = it->find("path");
                if (demo_path) {
                    DemoConfig new_d;
                    new_d.path = demo_path->as<std::string>();
                    demos.push_back(new_d);
                }
            }
        } else {
            std::cerr << "If key 'demos' exists, it needs to be a table array." << std::endl;
        }
    }
    std::cout << "Num demos: " << demos.size() << std::endl;
}
void Configuration::ReadConfigFile(std::ifstream& ifs)
{
    auto config = cpptoml::parse_file("config.toml");

    /* Patience */
    patience = config->get_qualified_as<int64_t>("general.patience").value_or(10);

    /* Authentication */
    // possible ??
    authentication = config->get_qualified_as<bool>("general.authentication").value_or(false);

    /* Demos table array */
    auto demos_tarr = config->get_table_array("demos");
    for (const auto& table : *demos_tarr)
    {
        // *table is a cpptoml::table
        auto path_opt = table->get_as<std::string>("path");
        assert(path_opt);
        DemoConfig new_demo_config;
        new_demo_config.path = *path_opt;
        demos.push_back(new_demo_config);
    }

    /* Login */
    auto login_path_opt = config->get_qualified_as<std::string>("login.path");
    login_path = *login_path_opt;

    login_patience = config->get_qualified_as<int64_t>("login.patience").value_or(10);

}
