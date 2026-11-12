#include "PluginLoader.hpp"

#include <iostream>
#include <filesystem>
#include <string>
#include <boost/program_options.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

int main(int argc, char* argv[]) {
    // Create the main logger for the host application
    auto logger = spdlog::stdout_color_mt("Host");

    logger->info("=========================================");
    logger->info(" C++20 Dynamic Plugin Architecture Demo  ");
    logger->info("=========================================");
    logger->info("");

    // Determine plugin path from CLI argument or default build location
    std::filesystem::path pluginPath;
    std::filesystem::path pluginFolder = "plugins";

    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help,h",
            "Show help message")
        ("plugin,p", boost::program_options::value<std::string>()->default_value("sample_plugin.so"),
            "Path to the plugin shared library");

    boost::program_options::variables_map vm;
    try {
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return 0;
        }

        if (vm.count("plugin")) {
            pluginPath = vm["plugin"].as<std::string>();
            logger->info("Using plugin path: {}", pluginPath.string());
        }
    } catch (const std::exception& ex) {
        std::cerr << "[Host Error] " << ex.what() << "\n";
        return 1;
    }

    try {
        std::cout << "[Host] Attempting to load plugin from: " << pluginPath << "\n";

        {
            // Scope block to verify RAII destruction and library unload
            std::cout << "[Host] Entering scoped block for plugin usage...\n";
            plugin_demo::PluginPtr plugin = plugin_demo::PluginLoader::load(pluginPath);

            // Convert this to logger output
            logger->info("[Host] Successfully loaded plugin!");
            logger->info("[Host]   - Plugin Name:    {}", plugin->name());
            logger->info("[Host]   - Plugin Version: {}", plugin->version());

            logger->info("[Host] Invoking plugin->execute()...");
            plugin->execute();

            double a = 5.0;
            double b = 10.0;
            double result = plugin->calculate(a, b);
            logger->info("[Host] Invoking plugin->calculate({0}, {1}) = {2}", a, b, result);

            logger->info("[Host] Exiting scoped block (plugin unique_ptr will now be destroyed)...");
        }

        logger->info("[Host] Scoped block exited. Plugin object and shared library unloaded safely via RAII.");

    } catch (const std::exception& ex) {
        std::cerr << "[Host Error] " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
